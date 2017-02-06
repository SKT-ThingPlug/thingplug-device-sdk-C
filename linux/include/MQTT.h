/**
 * @file MQTTClient.h
 *
 * @brief MQTT Client header for Linux
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#ifndef _MQTT_H_
#define _MQTT_H_

#include "oneM2M.h"

/*
 ****************************************
 * Enumerations
 ****************************************
 */


/*
 ****************************************
 * Structure Definition
 ****************************************
 */
typedef struct
{
	/** data **/
	char* data;
	/** length **/
	size_t len;
} Content;

extern Content* gContent;

/*
 ****************************************
 * Type Definition
 ****************************************
 */


/*
 ****************************************
 * Major Function
 ****************************************
 */
int MQTTSetCallbacks(tpMQTTConnectedCallback* cc, tpMQTTSubscribedCallback* sc, tpMQTTDisconnectedCallback* dc,
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac);

int MQTTAsyncCreate(char* host, int port, int keepalive, char* userName, char* password, int enableServerCertAuth, 
         char* subscribeTopic[], int subscribeTopicSize, char* publishTopic, char* enabledCipherSuites, int cleanSession, char* clientID);
         
int MQTTAsyncSubscribe(char* topic, int qos);

int MQTTAsyncPublishMessage(char* payload);

int MQTTAsyncDisconnect();

void MQTTAsyncDestroy();

int MQTTAsyncIsConnected();

#endif //_MQTT_H_

