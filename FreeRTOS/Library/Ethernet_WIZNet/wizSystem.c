#include "wizSystem.h"
#include <sys/time.h>
#include "stdlib.h"

static wiz_NetInfo NetInfo = {
	.mac = NETINFO_MAC_DEFAULT,
	.ip = NETINFO_IP_DEFAULT,
	.sn = NETINFO_SUBNETMASK_DEFAULT,
	.gw = NETINFO_GATEWAY_DEFAULT,
	.dns = NETINFO_DNS_DEFAULT,
	.dhcp = NETINFO_DHCP_DEFAULT
};

static uint32_t leaseTime;
static uint32_t randomKey;
static uint8_t SockStatus;
static time_t DHCPLeasedTime;

osPoolDef (NetMemPool,3,sockPool);
osPoolId NetMemPoolId;

osMutexDef (spiMutex);
osMutexId  (spiMutexId); // Mutex ID

osMutexDef (sockMutex);   // Declare mutex
osMutexId  (sockMutexId); // Mutex ID

extern osThreadId userTaskId;

#if NETINFO_NTP_USE
osThreadId tskNTPId;
void tskNTP(void const * argument);
#endif

/*For time library*/
static clock_t clk_count = 0;
static uint64_t clockCount = 0;
static time_t time_dat;

uint64_t getClock(void)
{
	return clockCount;
}
clock_t clock (void) {
	return (clk_count);
}

time_t __time64 (time_t * p) {
	return time_dat;
}

time_t getSystemTime(void){
	return time_dat;
}

/*
 * Allocate socket and os buffer
 */
sockPool* sockPoolAlloc(void)
{
	int i;
	sockPool *eth;
	osMutexWait(sockMutexId,osWaitForever);
	eth = (sockPool*)osPoolCAlloc(NetMemPoolId);
	if(eth != NULL){
		for(i = 0 ; i < _WIZCHIP_SOCK_NUM_ ; i++)
		{
			if(SockStatus & (1<<i))
				continue;
			else
			{
				SockStatus |= (1<<i);
				eth->sockNo = i;
				osMutexRelease(sockMutexId);
				return eth;
			}
		}
	}
	osPoolFree(NetMemPoolId,eth);
	osMutexRelease(sockMutexId);
	return eth;
}

/*
 * Free socket and os buffer
 */
void sockPoolFree(sockPool* SockPool)
{
	if(SockPool == NULL) return;
	if(SockPool->sockNo > _WIZCHIP_SOCK_NUM_) return;
	SockStatus &= (unsigned char)(~(1<<SockPool->sockNo));
	close(SockPool->sockNo);
	osPoolFree(NetMemPoolId,SockPool);
}

void spiMutexEnter(void)
{
	osMutexWait(spiMutexId,osWaitForever);
}

void spiMutexExit(void)
{
	osMutexRelease(spiMutexId);
}

void wizSystemIncTimeClock(void)
{
	clk_count++;
	clockCount++;
	if((clk_count%1000) == 0) time_dat++;
}

int _gettimeofday(struct timeval *tv,struct timezone *tzvp)
{
	tv->tv_sec = time_dat;
	tv->tv_usec = time_dat+getClock()%1000;
	tzvp->tz_dsttime = 540;
	tzvp->tz_dsttime = DST_NONE;
	return 0;
}
/*
 *Time update manually
 */
void wizSystemTimeUpdate(struct tm *time)
{
	time_dat = mktime(time);
}
/*For time library*/

/*
 *System initialize
 */
uint8_t wizSystemInit(void)
{
	uint8_t W5500SockBufSize[2][8] = {{2,2,2,2,2,2,2,2,},{2,2,2,2,2,2,2,2}};
	uint32_t tickStart;

	/* spi function register */
	reg_wizchip_spi_cbfunc(W5500ReadByte, W5500WriteByte);

	/* CS function register */
	reg_wizchip_cs_cbfunc(W5500Select, W5500DeSelect);

	/* Critical section */
	reg_wizchip_cris_cbfunc(spiMutexEnter,spiMutexExit);

	//W5500Select();

	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)W5500SockBufSize) == -1)
	{
#if WIZSYSTEM_DEBUG == 1
		printf("W5500 initialized fail.\r\n");
#endif
		return -1;
	}
	/*Set network information by default*/
	wizchip_setnetinfo(&NetInfo);
	tickStart = HAL_GetTick();
	do{//check phy status.
		randomKey++;
		if((HAL_GetTick() - tickStart) > NETINFO_LINK_TIMEOUT)
		{
#if WIZSYSTEM_DEBUG == 1
			printf("Ethernet Link timeout.\r\n");
#endif
			return -1;
		}
	}while(!isLinked());
	return 1;
}

void str_to_ip(const char* str, uint8_t * ip_from_dns)
{
    char* p = (char*)str;
    for(int i = 0; i < 4; i++) {
    	ip_from_dns[i] = atoi(p);
        p = strchr(p, '.');
        if (p == NULL) {
            break;
        }
        p++;
    }
}

int8_t DNSTimeOut(uint8_t * name, uint8_t * ip_from_dns, uint32_t timeout)
{
	int8_t ret;

    str_to_ip(name, ip_from_dns);
    char buf[17];
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip_from_dns[0], ip_from_dns[1], ip_from_dns[2], ip_from_dns[3]);
    if (strcmp(buf, name) == 0) {
        return 0;
    }

	sockPool *sockP = sockPoolAlloc();
	DNS_init(sockP->sockNo,sockP->netBuf);
	ret = DNS_run(NetInfo.dns,name,ip_from_dns);

	if(ret < 0)
	{
		printf("DNS timeout.\r\n");
	}
	sockPoolFree(sockP);
	return ret;
}

int8_t DHCPTimeOut(uint32_t timeout)
{
	uint32_t tickStart;
	uint8_t ret;
	sockPool *sockP = sockPoolAlloc();
	DHCP_init(sockP->sockNo,sockP->netBuf);
	tickStart = HAL_GetTick();
	do{
		ret = DHCP_run();
		if(ret == DHCP_IP_LEASED)
		{
			leaseTime = getDHCPLeasetime();
#if WIZSYSTEM_DEBUG
			printf(" - DHCP Success: DHCP Leased time : %d seconds.\r\n",leaseTime);
			wizchip_getnetinfo(&NetInfo);
			printf("IP Address: %d.%d.%d.%d\r\n",NetInfo.ip[0],NetInfo.ip[1],NetInfo.ip[2],NetInfo.ip[3]);
			printf("Subnet Mask: %d.%d.%d.%d\r\n",NetInfo.sn[0],NetInfo.sn[1],NetInfo.sn[2],NetInfo.sn[3]);
			printf("Gate Way: %d.%d.%d.%d\r\n",NetInfo.gw[0],NetInfo.gw[1],NetInfo.gw[2],NetInfo.gw[3]);
			printf("DNS Server: %d.%d.%d.%d\r\n",NetInfo.dns[0],NetInfo.dns[1],NetInfo.dns[2],NetInfo.dns[3]);
#endif
			leaseTime = leaseTime/2;
			sockPoolFree(sockP);
			return ret;
		}
	}while((HAL_GetTick() - tickStart) < timeout);
	sockPoolFree(sockP);
	return -1;
}

time_t NTPTimeOut(uint8_t * NTPServer, uint32_t timeout)
{
	uint32_t tickStart;
	time_t currentTime;
	int8_t ret;
	uint8_t dnsIP[4] = {0,};
	sockPool *sockP = sockPoolAlloc();
	ret = DNSTimeOut(NTPServer,dnsIP,NETINFO_DNS_TIMEOUT);
	if(ret < 0)//dns timeout;
	{
		sockPoolFree(sockP);
		return ret;
	}
	SNTP_init(sockP->sockNo,dnsIP,40,sockP->netBuf);
	tickStart = HAL_GetTick();
	do{
		ret = SNTP_run(&currentTime);
		if(ret == 1)
		{
			sockPoolFree(sockP);
			return currentTime;
		}
	}while((HAL_GetTick() - tickStart) < timeout);
	sockPoolFree(sockP);
	return -1;
}

uint8_t isLinked(void)
{
	return (getPHYCFGR() & PHYCFGR_LNK_ON);
}

void tskEthernetManager(void const * argument)
{
	uint8_t preLinkStatus;
	uint8_t curLinkStatus;
	int8_t ret;
	sockMutexId = osMutexCreate(osMutex(sockMutex));
	spiMutexId = osMutexCreate(osMutex(spiMutex));
	NetMemPoolId = osPoolCreate(osPool(NetMemPool));

#if NETINFO_NTP_USE
	
	osThreadDef(ntptask, tskNTP, osPriorityBelowNormal, 0, 512);
	tskNTPId = osThreadCreate(osThread(ntptask), NULL);
#endif //NETINFO_NTP_USE

	wizSystemInit();
#if WIZSYSTEM_DEBUG
	printf("TASK: Ethernet manager start.\r\n");
#endif
	while(1)
	{
		osDelay(1000);// Every sec
		curLinkStatus = isLinked();
		if(preLinkStatus != curLinkStatus)
		{
			preLinkStatus = curLinkStatus;
			if(curLinkStatus)
			{
				leaseTime = 0;
			}
			else
			{
#if WIZSYSTEM_DEBUG
				printf("Ethernet Unliked. W5500 Reinitialize.\r\n");
				wizSystemInit(); //W5500 initialize
#endif
			}
		}
		if(NetInfo.dhcp == NETINFO_DHCP && DHCPLeasedTime+leaseTime < time(NULL))
		{
			ret = DHCPTimeOut(NETINFO_DHCP_TIMEOUT);
			if(ret == DHCP_IP_LEASED)
			{
				DHCPLeasedTime = time(NULL);
				return;
			}
			else
			{
				leaseTime = 0;
			}
		}
	}
}

#if NETINFO_NTP_USE
void tskNTP(void const * argument)
{
	/*
	* Wait Signal(from Ethernet manager)
	* DNS request.
	* 	NTP Run() -> X: N Retry using the other NTP Server(DNS retry) || O: Time Update();
	*/
	time_t now;
	uint8_t NTPServerNum;
	uint8_t temp;
#if defined(NTP_SERVER_LIST)
	NTPServerNum = NTP_SERVER_NUM;
	uint8_t* NTPServerList[] = NTP_SERVER_LIST;
#else
	NTPServerNum = NTP_SERVER_NUM_DEFAULT;
	uint8_t* NTPServerList[] = NTP_SERVER_LIST_DEFAULT;
#endif

	while(1)
	{
		if(!isLinked() || (NetInfo.dhcp == NETINFO_DHCP && leaseTime == 0))
			continue;
		temp = 0;
		do{
			now = NTPTimeOut(NTPServerList[temp],NETINFO_NTP_TIMEOUT);
			if(now > 0)
			{
				DHCPLeasedTime = now - 2208988800 - time(NULL) + DHCPLeasedTime;
				time_dat = now - 2208988800;
				srand(now^randomKey);
#if WIZSYSTEM_DEBUG
				printf("NTP Time updated.\r\n");
				printfTime(9);
#endif
				osSignalSet(userTaskId,0x0001);
				osDelay(1000*60);//NETINFO_NTP_PERIODIC);
				break;
			}
			else
			{
#if WIZSYSTEM_DEBUG
				printf("Retry update time using other NTP Server. Retry Count = %d\r\n",temp);
#endif
				if(++temp == NETINFO_NTP_RETRY)
				{
				/*todo*/
				//Signal transmit to ethernet manager?
#if WIZSYSTEM_DEBUG
					printf("NTP Failed. Retry NTP time update after 10 minutes.\r\n");
#endif
					osDelay(600000);//Retry NTP time update after 10 minutes.
				}
			}
		}while(temp < NTPServerNum);
	}
}
#endif //NETINFO_NTP_USE

void printfTime(int8_t timezone)
{
	time_t now;
	struct tm * ts;
	char buf[80];
	now = time(NULL) + timezone*3600;
	printf("%llu\r\n",now);
	ts = localtime (& now);
	strftime (buf, sizeof (buf), "%a %Y-%m-%d %H:%M:%S%Z", ts);
	printf ("%s UTC %+d\r\n", buf,timezone);
}
