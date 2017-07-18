/*******************************************************************************
 * Copyright (c) 2017 SKT Corp.
 *******************************************************************************/

#ifndef __MQTT_S7G2_TLS_
#define __MQTT_S7G2_TLS_

#include "MQTTS7G2.h"

int s7g2_tls_read(Network*, unsigned char*, int, int);
int s7g2_tls_write(Network*, unsigned char*, int, int);
void s7g2_tls_disconnect(Network*);

void NewNetworkTLS(Network* n);
int ConnectNetworkTLS(Network* n, char* addr, unsigned int port);
void NetworkDisconnectTLS(Network* n);

#endif//__MQTT_S7G2_TLS_
