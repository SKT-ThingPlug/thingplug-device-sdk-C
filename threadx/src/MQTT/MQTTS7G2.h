/*******************************************************************************
 * Copyright (c) 2017 SKT Corp.
 *******************************************************************************/

#ifndef __MQTT_S7G2_
#define __MQTT_S7G2_

#include "nx_api.h"
#include "r_timer_api.h"


typedef struct Timer Timer;

struct Timer {
	unsigned long end_time;
};

void SysTickIntHandler(timer_callback_args_t * p_args);

char expired(Timer*);
void countdown_ms(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms(Timer*);

void InitTimer(Timer*);
typedef struct Network Network;

struct Network
{
	NX_TCP_SOCKET my_socket;
	NX_IP *my_ip;
	NX_PACKET_POOL *my_packet_pool;
	ULONG byte_ip;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

int s7g2_read(Network*, unsigned char*, int, int);
int s7g2_write(Network*, unsigned char*, int, int);
void s7g2_disconnect(Network*);

void NewNetwork(Network* n);
int ConnectNetwork(Network* n, char* addr, unsigned int port);
int IsNetworkConnected(Network* n);
void NetworkDisconnect(Network* n);

#endif//__MQTT_S7G2_
