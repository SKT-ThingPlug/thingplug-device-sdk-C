/**
 * @file oneM2M.c
 *
 * @brief oneM2M API
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#include <stdio.h>
#include <stddef.h>

#include "oneM2M.h"
#include "oneM2M/net/MQTTHandler.h"


/**
 * @brief set callback function
 * @param[in] cc connect callback
 * @param[in] sc subscribe callback
 * @param[in] dc disconnect callback
 * @param[in] clc connection lost callback
 * @param[in] mdc message delivered callback
 * @param[in] mac message arrived callback
 * @return the return code of the set callbacks result
 */
int tpMQTTSetCallbacks(tpMQTTConnectedCallback* cc, tpMQTTSubscribedCallback* sc, tpMQTTDisconnectedCallback* dc,
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac) {
    int rc = handleMQTTSetCallbacks(cc, sc, dc, clc, mdc, mac);
    return rc;
}

/**
 * @brief create mqtt
 * @param[in] host the hostname or ip address of the broker to connect to.
 * @param[in] port the network port to connect to.  Usually 1883.
 * @param[in] keepalive the number of seconds after which the broker should send a PING message to the client if no other messages have been exchanged in that time.
 * @param[in] userName authentication user name
 * @param[in] password authentication password
 * @param[in] enableServerCertAuth True/False option to enable verification of the server certificate
 * @param[in] subscribeTopic mqtt topic
 * @param[in] subscribeTopicSize mqtt topic size
 * @param[in] publishTopic publish topic
 * @param[in] clientID The client identifier passed to the server when the client connects to it.
 * @return the return code of the connection response
 */
int tpMQTTCreate(char* host, int port, int keepalive, char* userName, char* password,
        int enableServerCertAuth, char* subscribeTopic[], int subscribeTopicSize, char* publishTopic, char* clientID) {
    int rc = handleMQTTCreate(host, port, keepalive, userName, password, enableServerCertAuth,
        subscribeTopic, subscribeTopicSize, publishTopic, NULL, 1, clientID);
    return rc;
}

/**
 * @brief is connected
 * @return true : connected <-> false
 */
int tpMQTTIsConnected() {
    int rc = handleMQTTIsConnected();
    return rc;
}

/**
 * @brief yield
 * @param[in] timeout_ms time to yield
 * @return the return code of the yield response
 */
int tpMQTTYield(unsigned long timeout_ms) {
    return handleMQTTYield(timeout_ms);
}

/**
 * @brief disconnect mqtt
 */
int tpMQTTDisconnect() {
    int rc = handleMQTTDisconnect();
    return rc;
}

/**
 * @brief destroy
 */
void tpMQTTDestory() {
    handleMQTTDestroy();
}
