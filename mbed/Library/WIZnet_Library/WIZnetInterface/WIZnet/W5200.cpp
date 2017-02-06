/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mbed.h"
#include "mbed_debug.h"
#include "wiznet.h"
#include "DNSClient.h"

#ifdef USE_W5200

//Debug is disabled by default
#if 0
#define DBG(...) do{debug("%p %d %s ", this,__LINE__,__PRETTY_FUNCTION__); debug(__VA_ARGS__); } while(0);
//#define DBG(x, ...) debug("[WIZnet_Chip:DBG]"x"\r\n", ##__VA_ARGS__);
#define WARN(x, ...) debug("[WIZnet_Chip:WARN]"x"\r\n", ##__VA_ARGS__);
#define ERR(x, ...) debug("[WIZnet_Chip:ERR]"x"\r\n", ##__VA_ARGS__);
#else
#define DBG(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#endif

#if 1
#define INFO(x, ...) debug("[WIZnet_Chip:INFO]"x"\r\n", ##__VA_ARGS__);
#else
#define INFO(x, ...)
#endif

#define DBG_SPI 0

WIZnet_Chip* WIZnet_Chip::inst;

WIZnet_Chip::WIZnet_Chip(PinName mosi, PinName miso, PinName sclk, PinName _cs, PinName _reset):
    cs(_cs), reset_pin(_reset)
{
    spi = new SPI(mosi, miso, sclk);
    cs = 1;
    reset_pin = 1;
    inst = this;
}

WIZnet_Chip::WIZnet_Chip(SPI* spi, PinName _cs, PinName _reset):
    cs(_cs), reset_pin(_reset)
{
    this->spi = spi;
    cs = 1;
    reset_pin = 1;
    inst = this;
}

bool WIZnet_Chip::setip()
{
    reg_wr<uint32_t>(SIPR, ip);
    reg_wr<uint32_t>(GAR, gateway);
    reg_wr<uint32_t>(SUBR, netmask);
    return true;
}

bool WIZnet_Chip::setProtocol(int socket, Protocol p)
{
    if (socket < 0) {
        return false;
    }
    sreg<uint8_t>(socket, Sn_MR, p);
    return true;
}

bool WIZnet_Chip::connect(int socket, const char * host, int port, int timeout_ms)
{
    if (socket < 0) {
        return false;
    }
    sreg<uint8_t>(socket, Sn_MR, TCP);
    scmd(socket, CMD_OPEN);
    sreg_ip(socket, Sn_DIPR, host);
    sreg<uint16_t>(socket, Sn_DPORT, port);
    sreg<uint16_t>(socket, Sn_PORT, new_port());
    scmd(socket, CMD_CONNECT);
    Timer t;
    t.reset();
    t.start();
    while(!is_connected(socket)) {
        if (t.read_ms() > timeout_ms) {
            return false;
        }
    }
    return true;
}


bool WIZnet_Chip::is_fin_received(int socket)
{
    uint8_t tmpSn_SR;
    tmpSn_SR = sreg<uint8_t>(socket, Sn_SR);
    // packet sending is possible, when state is SOCK_CLOSE_WAIT.
    if (tmpSn_SR == SOCK_CLOSE_WAIT) {
        return true;
    }
    return false;
}

bool WIZnet_Chip::gethostbyname(const char* host, uint32_t* ip)
{
    uint32_t addr = str_to_ip(host);
    char buf[17];
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (addr>>24)&0xff, (addr>>16)&0xff, (addr>>8)&0xff, addr&0xff);
    if (strcmp(buf, host) == 0) {
        *ip = addr;
        return true;
    }
    DNSClient client;
    if(client.lookup(host)) {
        *ip = client.ip;
        return true;
    }
    return false;
}

bool WIZnet_Chip::disconnect()
{
    return true;
}

bool WIZnet_Chip::is_connected(int socket)
{
    if (sreg<uint8_t>(socket, Sn_SR) == SOCK_ESTABLISHED) {
        return true;
    }
    return false;
}

void WIZnet_Chip::reset()
{
    reset_pin = 1;
    reset_pin = 0;
    wait_us(2); // 2us
    reset_pin = 1;
    wait_ms(150); // 150ms
    
    reg_wr<uint8_t>(MR, 1<<7);

    reg_wr_mac(SHAR, mac);
}

bool WIZnet_Chip::close(int socket)
{
    if (socket < 0) {
        return false;
    }
    // if not connected, return
    if (sreg<uint8_t>(socket, Sn_SR) == SOCK_CLOSED) {
        return true;
    }
    if (sreg<uint8_t>(socket, Sn_MR) == TCP) {
        scmd(socket, CMD_DISCON);
    }
    scmd(socket, CMD_CLOSE);
    sreg<uint8_t>(socket, Sn_IR, 0xff);
    return true;
}

int WIZnet_Chip::wait_readable(int socket, int wait_time_ms, int req_size)
{
    if (socket < 0) {
        return -1;
    }
    Timer t;
    t.reset();
    t.start();
    while(1) {
        //int size = sreg<uint16_t>(socket, Sn_RX_RSR);
        // during the reading Sn_RX_RXR, it has the possible change of this register.
        // so read twice and get same value then use size information.
        int size, size2;
        do {
            size = sreg<uint16_t>(socket, Sn_RX_RSR);
            size2 = sreg<uint16_t>(socket, Sn_RX_RSR);
        } while (size != size2);

        if (size > req_size) {
            return size;
        }
        if (wait_time_ms != (-1) && t.read_ms() > wait_time_ms) {
            break;
        }
    }
    return -1;
}

int WIZnet_Chip::wait_writeable(int socket, int wait_time_ms, int req_size)
{
    if (socket < 0) {
        return -1;
    }
    Timer t;
    t.reset();
    t.start();
    while(1) {
        //int size = sreg<uint16_t>(socket, Sn_TX_FSR);
        // during the reading Sn_TX_FSR, it has the possible change of this register.
        // so read twice and get same value then use size information.
        int size, size2;
        do {
            size = sreg<uint16_t>(socket, Sn_TX_FSR);
            size2 = sreg<uint16_t>(socket, Sn_TX_FSR);
        } while (size != size2);
        
        if (size > req_size) {
            return size;
        }
        if (wait_time_ms != (-1) && t.read_ms() > wait_time_ms) {
            break;
        }
    }
    return -1;
}

int WIZnet_Chip::send(int socket, const char * str, int len)
{
    if (socket < 0) {
        return -1;
    }
    uint16_t base = 0x8000 + socket * 0x800;
    uint16_t ptr = sreg<uint16_t>(socket, Sn_TX_WR);
    uint16_t dst = base + (ptr&(0x800-1));    
    if ((dst + len) > (base+0x800)) {
        int len2 = base + 0x800 - dst;
        spi_write(dst, (uint8_t*)str, len2);
        spi_write(base, (uint8_t*)str+len2, len-len2);
    } else {
        spi_write(dst, (uint8_t*)str, len);
    }
    sreg<uint16_t>(socket, Sn_TX_WR, ptr + len);
    scmd(socket, CMD_SEND);
    return len;
}

int WIZnet_Chip::recv(int socket, char* buf, int len)
{
    if (socket < 0) {
        return -1;
    }
    uint16_t base = 0xc000 + socket * 0x800;
    uint16_t ptr = sreg<uint16_t>(socket, Sn_RX_RD);
    uint16_t src = base + (ptr&(0x800-1));    
    if ((src + len) > (base+0x800)) {
        int len2 = base + 0x800 - src;
        spi_read(src, (uint8_t*)buf, len2);
        spi_read(base, (uint8_t*)buf+len2, len-len2);
    } else {
        spi_read(src, (uint8_t*)buf, len);
    }
    sreg<uint16_t>(socket, Sn_RX_RD, ptr + len);
    scmd(socket, CMD_RECV);
    return len;
}

int WIZnet_Chip::new_socket()
{
    for(int s = 0; s < 8; s++) {
        if (sreg<uint8_t>(s, Sn_SR) == SOCK_CLOSED) {
            return s;
        }
    }
    return -1;
}

uint16_t WIZnet_Chip::new_port()
{
    uint16_t port = rand();
    port |= 49152;
    return port;
}

void WIZnet_Chip::scmd(int socket, Command cmd)
{
    sreg<uint8_t>(socket, Sn_CR, cmd);
    while(sreg<uint8_t>(socket, Sn_CR));
}

void WIZnet_Chip::spi_write(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    cs = 0;
    spi->write(addr >> 8);
    spi->write(addr & 0xff);
    spi->write((0x80 | ((len & 0x7f00) >> 8)));
    spi->write(len & 0xff);
    for(int i = 0; i < len; i++) {
        spi->write(buf[i]);
    }
    cs = 1;

#if DBG_SPI
    debug("[SPI]W %04x(%d)", addr, len);
    for(int i = 0; i < len; i++) {
        debug(" %02x", buf[i]);
        if (i > 16) {
            debug(" ...");
            break;
        }
    }
    debug("\r\n");
#endif    
}

void WIZnet_Chip::spi_read(uint16_t addr, uint8_t *buf, uint16_t len)
{
    cs = 0;
    spi->write(addr >> 8);
    spi->write(addr & 0xff);
    spi->write((0x00 | ((len & 0x7f00) >> 8)));
    spi->write(len & 0xff);
    for(int i = 0; i < len; i++) {
        buf[i] = spi->write(0);
    }
    cs = 1;

#if DBG_SPI
    debug("[SPI]R %04x(%d)", addr, len);
    for(int i = 0; i < len; i++) {
        debug(" %02x", buf[i]);
        if (i > 16) {
            debug(" ...");
            break;
        }
    }
    debug("\r\n");
    if ((addr&0xf0ff)==0x4026 || (addr&0xf0ff)==0x4003) {
        wait_ms(200);
    }
#endif    
}

uint32_t str_to_ip(const char* str)
{
    uint32_t ip = 0;
    char* p = (char*)str;
    for(int i = 0; i < 4; i++) {
        ip |= atoi(p);
        p = strchr(p, '.');
        if (p == NULL) {
            break;
        }
        ip <<= 8;
        p++;
    }
    return ip;
}

void printfBytes(char* str, uint8_t* buf, int len)
{
    printf("%s %d:", str, len);
    for(int i = 0; i < len; i++) {
        printf(" %02x", buf[i]);
    }
    printf("\n");  
}

void printHex(uint8_t* buf, int len)
{
    for(int i = 0; i < len; i++) {
        if ((i%16) == 0) {
            printf("%p", buf+i);
        }
        printf(" %02x", buf[i]);
        if ((i%16) == 15) {
            printf("\n");
        }
    }
    printf("\n");
}

void debug_hex(uint8_t* buf, int len)
{
    for(int i = 0; i < len; i++) {
        if ((i%16) == 0) {
            debug("%p", buf+i);
        }
        debug(" %02x", buf[i]);
        if ((i%16) == 15) {
            debug("\n");
        }
    }
    debug("\n");
}

#endif
