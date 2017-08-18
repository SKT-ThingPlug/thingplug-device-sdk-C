

#include "NTPClient.h"
#include "wizSystem.h"
#include "SKTtpDebug.h"
#include "MQTTFreeRTOS.h"

#include <time.h>

#define NTP_PORT 123
#define NTP_CLIENT_PORT 0 //Random port
#define NTP_TIMESTAMP_DELTA 2208988800ull //Diff btw a UNIX timestamp (Starting Jan, 1st 1970) and a NTP timestamp (Starting Jan, 1st 1900)
#define NTP_IP "211.233.40.78"
//#define NTP_IP "192.168.0.13"
#define NTP_DEFAULT_TIMEOUT 4000

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

///NTP client results
enum NTPResult
{
	NTP_DNS, ///<Could not resolve name
	NTP_PRTCL, ///<Protocol error
	NTP_TIMEOUT, ///<Connection timeout
	NTP_CONN, ///<Connection error
	NTP_OK = 0, ///<Success
};

static sockPool	*pSockPool;
static int gOffset;
static uint8_t hostIp[4];
int NTPSocket(void)
{
	int retVal = -1;
	if((retVal = DNSTimeOut((uint8_t*)NTP_IP, hostIp, NETINFO_DNS_TIMEOUT)) < 0)
	{
		goto exit;
	}

	pSockPool = sockPoolAlloc();

	if((retVal = socket(pSockPool->sockNo, Sn_MR_UDP, -1, 0)) < SOCK_ERROR)
	{
		sockPoolFree(pSockPool);
		goto exit;
	}
	SKTDebugPrint(LOG_LEVEL_INFO, "NTPSocket Success!!");
	pSockPool->sockNo = retVal;
exit:
	return retVal;
}

int setTime(void)
{
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
	pkt.txTm_s = NTOHL( NTP_TIMESTAMP_DELTA + time(NULL) ); //WARN: We are in LE format, network byte order is BE

	pkt.refTm_f = pkt.origTm_f = pkt.rxTm_f = pkt.txTm_f = 0;

	int sentLen = 0;
	int rc;
	uint8_t *buffer = (uint8_t *)&pkt;
	int len = sizeof(NTPPacket);
	Timer t1;
	TimerInit(&t1);
	TimerCountdownMS(&t1, 2000);
	do
	{
		if(TimerIsExpired(&t1) ) {
			SKTDebugPrint(LOG_LEVEL_INFO, "NTPSocket timeout!!");
		    break;
		}

		rc = 0;

		rc = sendto(pSockPool->sockNo,buffer + sentLen, len - sentLen, hostIp, NTP_PORT);

		if (rc > 0)
			sentLen += rc;
		else if (rc < 0)
		{
			sentLen = rc;
			break;
		}
	} while (sentLen < len);

	if( sentLen < len ) {
		disconnect(pSockPool->sockNo);
		sockPoolFree(pSockPool);
		return NTP_PRTCL;
	}

	int recvLen = 0;
	len = sizeof(NTPPacket);
	uint16_t port = -1;
	Timer t2;
	TimerInit(&t2);
	TimerCountdownMS(&t2, 2000);
	do
	{
		if(TimerIsExpired(&t2) ) {
			SKTDebugPrint(LOG_LEVEL_INFO, "NTPSocket timeout!!");
			break;
		}
		rc = 0;
		rc = recvfrom(pSockPool->sockNo, buffer + recvLen, len - recvLen, hostIp, &port);
		if (rc > 0)
			recvLen += rc;
		else if (rc < 0)
		{
			recvLen = rc;
			break;
		}
	} while (recvLen < len);

	if( recvLen < len ) {
		disconnect(pSockPool->sockNo);
		sockPoolFree(pSockPool);
		return NTP_PRTCL;
	}

	if( pkt.stratum == 0)  //Kiss of death message : Not good !
	{
		disconnect(pSockPool->sockNo);
		sockPoolFree(pSockPool);
		return NTP_PRTCL;
	}

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
	uint32_t destTm_s = (NTP_TIMESTAMP_DELTA + time(NULL));
	int64_t offset = ( (int64_t)( pkt.rxTm_s - pkt.origTm_s ) + (int64_t) ( pkt.txTm_s - destTm_s ) ) / 2; //Avoid overflow
	//Set time accordingly
	//set_time( time(NULL) + offset );
	gOffset = offset;
	disconnect(pSockPool->sockNo);
	sockPoolFree(pSockPool);
	SKTDebugPrint(LOG_LEVEL_INFO, "NTPSocket Success!!");
	return NTP_OK;
}

int get_npt_offset (void)
{
	return gOffset;
}

