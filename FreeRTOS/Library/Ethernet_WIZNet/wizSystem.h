#ifndef __WIZSYSTEM_H_
#define __WIZSYSTEM_H_


#include <stdio.h>
#include <stdlib.h>
#include "wizchip_conf.h"
#include "cmsis_os.h"
#include "time.h"
#include "socket.h"
#include "W5500HardwareDriver.h"
#include "dhcp.h"
#include "sntp.h"
#include "dns.h"
#include "oneM2MConfig.h"

#define WIZSYSTEM_DEBUG					0

#ifdef MAC_ADDRESS
#define NETINFO_MAC_DEFAULT				MAC_ADDRESS
#else
#define NETINFO_MAC_DEFAULT				{0x00,0x08,0xdc,0x1e,0xcc,0xf2}
#endif
#define NETINFO_IP_DEFAULT				{222,98,173,249}
#define NETINFO_DNS_DEFAULT				{8,8,8,8}		//{168,126,63,1}
#define NETINFO_GATEWAY_DEFAULT			{222,98,173,254}
#define NETINFO_SUBNETMASK_DEFAULT		{255,255,255,192}
#define NETINFO_DHCP_DEFAULT			NETINFO_DHCP

#define NETINFO_NTP_USE					0
#define NETINFO_LINK_TIMEOUT			3000
#define NETINFO_DHCP_TIMEOUT			10000	// Timeout Value for DHCP 10 sec
#define NETINFO_DHCP_RETRY				5	// Timeout Value for DHCP 10 sec
#define NETINFO_DNS_TIMEOUT				3000	// Timeout Value for DNS 3 sec
#define NETINFO_NTP_PERIODIC			3600000*24
#define NETINFO_NTP_TIMEOUT				5000	//Timeout Value For NTP 5 sec
#define NETINFO_NTP_RETRY				5	//Timeout Value For NTP 5 sec

#define NETINFO_SOCKET_BUFFER_SIZE_DEFAULT	2048

#if defined(NTP_SERVER_LIST) && !defined(NTP_SERVER_NUM)
#error "NTP_SERVER_NUM must be defined"
#endif

#define NTP_SERVER_LIST_DEFAULT			{"kr.pool.ntp.org","time.nuri.net","ntp.kornet.net","time.kriss.re.kr","time.bora.net"}
#define NTP_SERVER_NUM_DEFAULT			5

#define WIZ_ERR_SOCKET_ALLOC		-0x9100

typedef struct{
  uint8_t sockNo;
  char netBuf[NETINFO_SOCKET_BUFFER_SIZE_DEFAULT];
}sockPool;

/*
 * Allocate socket 
 */
sockPool* sockPoolAlloc(void);

/*
 * Free socket 
 */
void sockPoolFree(sockPool* SockPool);

/*
 * W5500 SPI Enter to mutex section
 */
void spiMutexEnter(void);

/*
 * W5500 SPI Exit from mutex section
 */
void spiMutexExit(void);

/*
 * Time Clock update
 */
void wizSystemIncTimeClock(void);

/*
 *Current time update.
 */
void wizSystemTimeUpdate(struct tm *time);

/*
 *System initialize
 */
uint8_t wizSystemInit(void);

/*
 * Check whether ethernet is linked or link-down
 */
uint8_t isLinked(void);

/*
 * Print current time.
 */
void printfTime(int8_t timezone);

/*
 *Function: DHCP
 */
int8_t DHCPTimeOut(uint32_t timeout);

/*
 *Function: NTP
 */
time_t NTPTimeOut(uint8_t * NTPServer, uint32_t timeout);

/*
 *Function: DNS
 */
int8_t DNSTimeOut(uint8_t * name, uint8_t * ip_from_dns, uint32_t timeout);

/*
 * OS Task - Ethernet manager
 */
void tskEthernetManager(void const * argument);

/*
 * OS Task - NTP
 */
#if NETINFO_NTP_USE
void tskNTP(void const * argument);
#endif //NETINFO_NTP_USE

#endif
