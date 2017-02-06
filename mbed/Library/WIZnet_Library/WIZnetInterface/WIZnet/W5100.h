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
 *
 */

#pragma once

#include "mbed.h"
#include "mbed_debug.h"

#define TEST_ASSERT(A) while(!(A)){debug("\n\n%s@%d %s ASSERT!\n\n",__PRETTY_FUNCTION__,__LINE__,#A);exit(1);};

#define DEFAULT_WAIT_RESP_TIMEOUT 500

enum Protocol {
    CLOSED = 0,
    TCP    = 1,
    UDP    = 2,
};

enum Command {
	CMD_OPEN      = 0x01,
	CMD_LISTEN    = 0x02,
	CMD_CONNECT   = 0x04,
	CMD_DISCON    = 0x08,
	CMD_CLOSE     = 0x10,
	CMD_SEND      = 0x20,
	CMD_SEND_MAC  = 0x21, 
	CMD_SEND_KEEP = 0x22,
	CMD_RECV      = 0x40,
    
};

enum Interrupt {
    INT_CON     = 0x01,
    INT_DISCON  = 0x02,
    INT_RECV    = 0x04,
    INT_TIMEOUT = 0x08,
    INT_SEND_OK = 0x10,
};

enum Status {
    SOCK_CLOSED      = 0x00,
    SOCK_INIT        = 0x13,
    SOCK_LISTEN      = 0x14,
    SOCK_ESTABLISHED = 0x17,
    SOCK_CLOSE_WAIT  = 0x1c,
    SOCK_UDP         = 0x22,
};

#define MAX_SOCK_NUM 4

#define MR        0x0000
#define GAR       0x0001
#define SUBR      0x0005
#define SHAR      0x0009
#define SIPR      0x000f

// W5100 socket
#define Sn_MR         0x0400
#define Sn_CR         0x0401
#define Sn_IR         0x0402
#define Sn_SR         0x0403
#define Sn_PORT       0x0404
#define Sn_DIPR       0x040c
#define Sn_DPORT      0x0410
//#define Sn_RXBUF_SIZE 0x401e
//#define Sn_TXBUF_SIZE 0x401f
#define Sn_TX_FSR     0x0420
#define Sn_TX_WR      0x0424
#define Sn_RX_RSR     0x0426
#define Sn_RX_RD      0x0428

class WIZnet_Chip {
public:
    /*
    * Constructor
    *
    * @param spi spi class
    * @param cs cs of the W5200
    * @param reset reset pin of the W5200
    */
	WIZnet_Chip(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName reset);
	WIZnet_Chip(SPI* spi, PinName cs, PinName reset);

    /*
    * Connect the W5200 module to the ssid contained in the constructor.
    *
    * @return true if connected, false otherwise
    */
    bool setip();

    /*
    * Disconnect the W5200 module from the access point
    *
    * @ returns true if successful
    */
    bool disconnect();

    /*
    * Open a tcp connection with the specified host on the specified port
    *
    * @param host host (can be either an ip address or a name. If a name is provided, a dns request will be established)
    * @param port port
    * @ returns true if successful
    */
    bool connect(int socket, const char * host, int port, int timeout_ms = 10*1000);

    /*
    * Set the protocol (UDP or TCP)
    *
    * @param p protocol
    * @ returns true if successful
    */
    bool setProtocol(int socket, Protocol p);

    /*
    * Reset the W5100 module
    */
    void reset();
    
   
    int wait_readable(int socket, int wait_time_ms, int req_size = 0);

    int wait_writeable(int socket, int wait_time_ms, int req_size = 0);

    /*
    * Check if a tcp link is active
    *
    * @returns true if successful
    */
    bool is_connected(int socket);

	/*
    * Check if FIN received.
    *
    * @returns true if successful
    */
    bool is_fin_received(int socket);
    
    /*
    * Close a tcp connection
    *
    * @ returns true if successful
    */
    bool close(int socket);

    /*
    * @param str string to be sent
    * @param len string length
    */
    int send(int socket, const char * str, int len);

    int recv(int socket, char* buf, int len);

    /*
    * Return true if the module is using dhcp
    *
    * @returns true if the module is using dhcp
    */
    bool isDHCP() {
        return dhcp;
    }

    bool gethostbyname(const char* host, uint32_t* ip);

    static WIZnet_Chip * getInstance() {
        return inst;
    };

    int new_socket();
    uint16_t new_port();
    void scmd(int socket, Command cmd);

    template<typename T>
    void sreg(int socket, uint16_t addr, T data) {
        reg_wr<T>(addr+0x100*socket, data);
    }

    template<typename T>
    T sreg(int socket, uint16_t addr) {
        return reg_rd<T>(addr+0x100*socket);
    }

    template<typename T>
    void reg_wr(uint16_t addr, T data) {
        uint8_t buf[sizeof(T)];
        *reinterpret_cast<T*>(buf) = data;
        for(int i = 0; i < sizeof(buf)/2; i++) { //  Little Endian to Big Endian
            uint8_t t = buf[i];
            buf[i] = buf[sizeof(buf)-1-i];
            buf[sizeof(buf)-1-i] = t;
        }
        spi_write(addr, buf, sizeof(buf));
    }

    template<typename T>
    T reg_rd(uint16_t addr) {
        uint8_t buf[sizeof(T)];
        spi_read(addr, buf, sizeof(buf));
        for(int i = 0; i < sizeof(buf)/2; i++) { // Big Endian to Little Endian
            uint8_t t = buf[i];
            buf[i] = buf[sizeof(buf)-1-i];
            buf[sizeof(buf)-1-i] = t;
        }
        return *reinterpret_cast<T*>(buf);
    }

    void reg_rd_mac(uint16_t addr, uint8_t* data) {
        spi_read(addr, data, 6);
    }

    void reg_wr_ip(uint16_t addr, const char* ip) {
        uint8_t buf[4];
        char* p = (char*)ip;
        for(int i = 0; i < 4; i++) {
            buf[i] = atoi(p);
            p = strchr(p, '.');
            if (p == NULL) {
                break;
            }
            p++;
        }
        spi_write(addr, buf, sizeof(buf));
    }

    void sreg_ip(int socket, uint16_t addr, const char* ip) {
        reg_wr_ip(addr+0x100*socket, ip);
    }

protected:
    uint8_t mac[6];
    uint32_t ip;
    uint32_t netmask;
    uint32_t gateway;
    uint32_t dnsaddr;
    bool dhcp;

    static WIZnet_Chip* inst;

    void reg_wr_mac(uint16_t addr, uint8_t* data) {
        spi_write(addr, data, 6);
    }

    void spi_write(uint16_t addr, const uint8_t *buf, uint16_t len);
    void spi_read(uint16_t addr, uint8_t *buf, uint16_t len);
    SPI* spi;
    DigitalOut cs;
    DigitalOut reset_pin;
};

extern uint32_t str_to_ip(const char* str);
extern void printfBytes(char* str, uint8_t* buf, int len);
extern void printHex(uint8_t* buf, int len);
extern void debug_hex(uint8_t* buf, int len);
