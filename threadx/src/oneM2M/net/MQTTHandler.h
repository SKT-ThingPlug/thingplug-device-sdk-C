/**
 * @file MQTT_handlder.h
 *
 * @brief MQTT handler header used MQTT paho
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#ifndef _MQTT_HANDLER_H_
#define _MQTT_HANDLER_H_

#include "oneM2M/oneM2M.h"

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
int handleMQTTSetCallbacks(tpMQTTConnectedCallback* cc, tpMQTTSubscribedCallback* sc, tpMQTTDisconnectedCallback* dc,
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac);

int handleMQTTCreate(char* host, int port, int keepalive, char* userName, char* password, int enableServerCertAuth,
         char* subscribeTopic[], int subscribeTopicCnt, char* publishTopic, char* enabledCipherSuites, int cleanSession, char* clientID);

int handleMQTTSubscribe(char* topic, int qos);

int handleMQTTPublishMessage(char* payload);

int handleMQTTYield(unsigned long timeout_ms);

int handleMQTTIsConnected(void);

int handleMQTTDisconnect(void);

void handleMQTTDestroy(void);

#endif //_MQTT_HANDLER_H_
