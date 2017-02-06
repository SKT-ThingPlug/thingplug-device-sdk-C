#if !defined(MQTTETHERNETTLS_H)
#define MQTTETHERNETTLS_H

#include "MQTTFreeRTOS.h"

int		EthernetTLS_read(Network*, unsigned char*, int, int);
int		EthernetTLS_write(Network*, unsigned char*, int, int);
void	EthernetTLS_disconnect(Network*);

void	NetworkInitTLS(Network*);
int		NetworkConnectTLS(Network*, char*, int);
#ifdef USE_ETHERNET_LIB
void	NetworkDisconnectTLS(Network*);
#endif

#endif
