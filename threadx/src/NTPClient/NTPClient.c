/*******************************************************************************
 * Copyright (c) 2017 SKT Corp.
 *******************************************************************************/

#include "NTPClient.h"
#include "nx_api.h"
#include "common_data.h"
#include "nx_dns.h"
#include "oneM2M/SKTtpDebug.h"

#define HTONS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
#define NTOHS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))

#define HTONL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
		((((unsigned long)(n) & 0xFF00)) << 8) | \
		((((unsigned long)(n) & 0xFF0000)) >> 8) | \
		((((unsigned long)(n) & 0xFF000000)) >> 24))

#define NTOHL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
		((((unsigned long)(n) & 0xFF00)) << 8) | \
		((((unsigned long)(n) & 0xFF0000)) >> 8) | \
		((((unsigned long)(n) & 0xFF000000)) >> 24))

#include <stdint.h>
#define NTP_TIMESTAMP_DELTA 2208988800ull //Diff btw a UNIX timestamp (Starting Jan, 1st 1970) and a NTP timestamp (Starting Jan, 1st 1900)
typedef struct _NTPPacket //See RFC 4330 for Simple NTP
{
    //LSb first
    unsigned mode : 3;
    unsigned vn : 3;
    unsigned li : 2;

    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    //32 bits header

    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;

    uint32_t refTm_s;
    uint32_t refTm_f;
    uint32_t origTm_s;
    uint32_t origTm_f;
    uint32_t rxTm_s;
    uint32_t rxTm_f;
    uint32_t txTm_s;
    uint32_t txTm_f;
} NTPPacket;

static int gOffset;

int NTPSetTimer(void)
{
    int result;
    NX_UDP_SOCKET udp_socket;
    nx_udp_socket_create(&g_ip0, &udp_socket, "NTP Socket", NX_IP_NORMAL, NX_DONT_FRAGMENT, 255,  30);
    if ( result != NX_SUCCESS )
    {
        SKTDebugPrint(LOG_LEVEL_INFO, "NTP Socket Create error");
        return -1;
    }
    SKTDebugPrint(LOG_LEVEL_INFO, "NTP Socket Create ok");


    result = nx_udp_socket_bind( &udp_socket, NX_ANY_PORT, 2000 );
    if ( result != NX_SUCCESS )
    {
        SKTDebugPrint(LOG_LEVEL_INFO, "NTP Socket Bind error");
        return -1;
    }
    SKTDebugPrint(LOG_LEVEL_INFO, "NTP Socket Bind ok");

    ULONG server_address;
    server_address = IP_ADDRESS(211,233,40,78);

    NTPPacket pkt;
    //Prepare NTP Packet:
    pkt.li = 0; //Leap Indicator : No warning
    pkt.vn = 4; //Version Number : 4
    pkt.mode = 3; //Client mode
    pkt.stratum = 0; //Not relevant here
    pkt.poll = 0; //Not significant as well
    pkt.precision = 0; //Neither this one is

    pkt.rootDelay = 0; //Or this one
    pkt.rootDispersion = 0; //Or that one
    pkt.refId = 0; //...

    pkt.refTm_s = 0;
    pkt.origTm_s = 0;
    pkt.rxTm_s = 0;
    pkt.txTm_s = NTOHL( NTP_TIMESTAMP_DELTA ); //WARN: We are in LE format, network byte order is BE

    pkt.refTm_f = pkt.origTm_f = pkt.rxTm_f = pkt.txTm_f = 0;
    uint8_t *buffer = (uint8_t *)&pkt;
    NX_PACKET       *packet_ptr;
    nx_packet_allocate(&g_packet_pool0, &packet_ptr, NX_UDP_PACKET, 2000);
    nx_packet_data_append(packet_ptr, buffer, sizeof(NTPPacket), &g_packet_pool0, 2000);
    nx_udp_socket_send(&udp_socket, packet_ptr, server_address, 123);
    if ( result != NX_SUCCESS )
    {
        SKTDebugPrint(LOG_LEVEL_INFO, "NTP Time Request error");
        return -1;
    }
    result = nx_packet_release(packet_ptr);
    SKTDebugPrint(LOG_LEVEL_INFO, "NTP Time Request ok");

    int len = sizeof(NTPPacket);
    ULONG copied_idx = 0;
    Timer t;
    countdown_ms(&t , 2000);

    NX_PACKET *read_packet_ptr = NULL;
    ULONG prepend_offset = 0;
    while(len != 0)
    {
        ULONG left_len;
        ULONG recv_len = 0;
        ULONG bytes_copied = 0;

        if(prepend_offset == 0){
            nx_udp_socket_receive(&udp_socket, &read_packet_ptr, 2000);
        }
        nx_packet_length_get(read_packet_ptr, &recv_len);
        left_len = recv_len - prepend_offset;
        if(recv_len == 0){
            if(expired(&t))break;
        }
        if( left_len >= len ) {
            nx_packet_data_extract_offset(read_packet_ptr, prepend_offset, &buffer[copied_idx], len, &bytes_copied);
            if( left_len == bytes_copied ){
                prepend_offset = 0;
                nx_packet_release(read_packet_ptr);
                read_packet_ptr = NULL;
            } else {
                prepend_offset += bytes_copied;
            }
        } else {
            nx_packet_data_extract_offset(read_packet_ptr, prepend_offset, &buffer[copied_idx], left_len, &bytes_copied);
            prepend_offset = 0;
            nx_packet_release(read_packet_ptr);
            read_packet_ptr = NULL;
        }
        len -= bytes_copied;
        copied_idx += bytes_copied;
    }
    if( len != 0 ) {
        SKTDebugPrint(LOG_LEVEL_INFO, "NTP Time Receive error");
        return -1;
    }
    SKTDebugPrint(LOG_LEVEL_INFO, "NTP Time Receive ok");

    //Correct Endianness
    pkt.refTm_s = NTOHL( pkt.refTm_s );
    pkt.refTm_f = NTOHL( pkt.refTm_f );
    pkt.origTm_s = NTOHL( pkt.origTm_s );
    pkt.origTm_f = NTOHL( pkt.origTm_f );
    pkt.rxTm_s = NTOHL( pkt.rxTm_s );
    pkt.rxTm_f = NTOHL( pkt.rxTm_f );
    pkt.txTm_s = NTOHL( pkt.txTm_s );
    pkt.txTm_f = NTOHL( pkt.txTm_f );

    //Compute offset, see RFC 4330 p.13
    uint32_t destTm_s = NTP_TIMESTAMP_DELTA;
    int64_t offset = ( (int64_t)( pkt.rxTm_s - pkt.origTm_s ) + (int64_t) ( pkt.txTm_s - destTm_s ) ) / 2;
    gOffset = offset;

    return 0;
}

int get_npt_offset (void)
{
    return gOffset;
}
