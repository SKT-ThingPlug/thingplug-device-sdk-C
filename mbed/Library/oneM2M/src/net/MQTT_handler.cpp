/**
 * @file MQTT_handlder.c
 *
 * @brief MQTT handler used MQTT paho
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include <mbed.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "MQTT_handler.h"
#include "SKTtpDebug.h"

#ifdef __cplusplus
}
#endif

#include "MQTTClient_handler.h"

#include "oneM2M.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace MQTT
{

char							g_szPublishTopic[MQTT_TOPIC_MAX_LENGTH];
char							g_szSubscribeTopic[MQTT_SUBSCRIBE_TOPIC_MAX_CNT][MQTT_TOPIC_MAX_LENGTH];

MQTTEthernetExt*				g_pNetwork = NULL;
MQTTClient_handler*				g_pMQTTClient = NULL;

tpMQTTConnectedCallback*		g_pConnectedCallback;
tpMQTTSubscribedCallback*		g_pSubscribedCallback;
tpMQTTDisconnectedCallback*		g_pDisconnectedCallback;
tpMQTTConnectionLostCallback*	g_pConnectionLostCallback;
tpMQTTMessageDeliveredCallback*	g_pMessageDeliveredCallback;
tpMQTTMessageArrivedCallback*	g_pMessageArrivedCallback;

Content* gContent = NULL;

void CallConnectedCallback(int result)
{
	if(g_pConnectedCallback)	{ g_pConnectedCallback(result); }
}

void CallSubscribedCallback(int result)
{
	if(g_pSubscribedCallback)	{ g_pSubscribedCallback(result); }
}

void CallDisconnectedCallback(int result)
{
	if(g_pDisconnectedCallback)	{ g_pDisconnectedCallback(result); }
}

void CallConnectionLostCallback(char *cause)
{
	if(g_pConnectionLostCallback)	{ g_pConnectionLostCallback(cause); }
}

void CallMessageArrivedCallback(MessageData& md)
{
	SKTDebugPrint(LOG_LEVEL_INFO, "CallMessageArrivedCallback()");

	if(md.message.dup)
	{
		return;
	}

	if(g_pMessageArrivedCallback)
	{
		Message& message = md.message;
		g_pMessageArrivedCallback(md.topicName.lenstring.data, (char*)message.payload, message.payloadlen);
	}
}

void CallMessageDeliveredCallback()
{
	if(g_pMessageDeliveredCallback)	{ g_pMessageDeliveredCallback(0); }
}

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
int handleMQTTSetCallbacks(tpMQTTConnectedCallback* cc, tpMQTTSubscribedCallback* sc, tpMQTTDisconnectedCallback* dc,
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac)
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTSetCallbacks()");

	if(mac == NULL)
	{
		return FAILURE;
	}

	g_pConnectedCallback = cc;
	g_pSubscribedCallback = sc;
	g_pDisconnectedCallback = dc;
	g_pConnectionLostCallback = clc;
	g_pMessageDeliveredCallback = mdc;
	g_pMessageArrivedCallback = mac;

	return SUCCESS;
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
 * @param[in] subscribeTopicCnt mqtt topic count
 * @param[in] publishTopic publish topic
 * @param[in] enabledCipherSuites cipher format. If this setting is ommitted, its default value will be "ALL".
 * @param[in] cleanSession if cleanSession=true, then the previous session information is cleared.
 * @param[in] clientID The client identifier(no longer 23 characters).
 * @return the return code of the connection response
 */
int handleMQTTCreate(char* host, int port, int keepalive, char* userName, char* password, int enableServerCertAuth,
         char* subscribeTopic[], int subscribeTopicCnt, char* publishTopic, char* enabledCipherSuites, int cleanSession, char* clientID)
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTCreate()");
	if(!enableServerCertAuth)	{ SKTDebugPrint(LOG_LEVEL_INFO, "Use normal socket, host:%s, port:%d", host, port); }
	else						{ SKTDebugPrint(LOG_LEVEL_INFO, "Use secure(TLS) socket, host:%s, port:%d", host, port); }

	if(subscribeTopicCnt > MQTT_SUBSCRIBE_TOPIC_MAX_CNT)
	{
		SKTDebugPrint(LOG_LEVEL_ERROR, "Too many MQTT topics.");
		return FAILURE;
	}

	if(g_pMQTTClient == NULL)
	{
		if(!enableServerCertAuth)
		{
			g_pNetwork = new MQTTEthernetExt();
			g_pMQTTClient = new MQTTClient_handler(*g_pNetwork);
		}
		else
		{
#ifdef IMPORT_TLS_LIB
			g_pNetwork = new MQTTEthernetTLS();
			g_pMQTTClient = new MQTTClient_handler(*(MQTTEthernetTLS*)g_pNetwork);
#else
			SKTDebugPrint(LOG_LEVEL_ERROR, "Not support TLS socket.");
			return FAILURE;
#endif
		}
	}

	// get and display client network info
	EthernetInterface& eth = g_pNetwork->getEth();
	SKTDebugPrint(LOG_LEVEL_INFO, "IP address is %s", eth.getIPAddress());
	SKTDebugPrint(LOG_LEVEL_INFO, "MAC address is %s", eth.getMACAddress());
	SKTDebugPrint(LOG_LEVEL_INFO, "Gateway address is %s", eth.getGateway());
	SKTDebugPrint(LOG_LEVEL_INFO, "Subnet Mask address is %s", eth.getNetworkMask());

	int rc = SUCCESS;
	uint8_t retry = 0;

	while(MQTT_RETRY_CNT > retry)
	{
		if((rc = g_pNetwork->connect(host, port)) == 0)
		{
			break;
		}

		SKTDebugPrint(LOG_LEVEL_ERROR, "Ethernet connect retry.");
		retry++;
		wait_ms(MQTT_RETRY_INTERVAL);
	}
	if(rc != 0)
	{
		SKTDebugPrint(LOG_LEVEL_ERROR, "Ethernet connect fail! return = %d, host = %s, port = %d", rc, host, port);
		return rc;
	}

	char uniqueid[32] = {0,};
	if(clientID == NULL || strlen(clientID) <= 0)
	{
		char clientID_prefix[] = MQTT_UNIQUE_ID_PREFIX;
		int randNumber = rand();		// for unique id
		sprintf(uniqueid, "%s%d", clientID_prefix, randNumber);
		clientID = uniqueid;
	}
	SKTDebugPrint(LOG_LEVEL_DEBUG, "clientID = %s", clientID);

	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = clientID;
	connectData.keepAliveInterval = keepalive;
	if(userName)	{ connectData.username.cstring = userName; }
	if(password)	{ connectData.password.cstring = password; }
	connectData.cleansession = cleanSession;

	retry = 0;
	while(MQTT_RETRY_CNT > retry)
	{
		if((rc = g_pMQTTClient->connect(connectData)) == SUCCESS)
		{
			break;
		}

		SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT connect retry.");
		retry++;
		wait_ms(MQTT_RETRY_INTERVAL);
	}
	if(rc != SUCCESS)
	{
		SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT connect Failed. result : %d", rc);
		CallConnectedCallback(FAILURE);
		return rc;
	}

	SKTDebugPrint(LOG_LEVEL_INFO, "MQTT Connected");
	CallConnectedCallback(SUCCESS);

	memset(g_szPublishTopic, 0, MQTT_TOPIC_MAX_LENGTH);
	memcpy(g_szPublishTopic, publishTopic, strlen(publishTopic));

	for(int nLoop1 = 0 ; nLoop1 < subscribeTopicCnt ; nLoop1++)
	{
		memset(g_szSubscribeTopic[nLoop1], 0, MQTT_TOPIC_MAX_LENGTH);
		memcpy(g_szSubscribeTopic[nLoop1], subscribeTopic[nLoop1], strlen(subscribeTopic[nLoop1]));

		SKTDebugPrint(LOG_LEVEL_INFO, "MQTT Request Subscribe topic : %s", g_szSubscribeTopic[nLoop1]);
		if((rc = g_pMQTTClient->subscribe(g_szSubscribeTopic[nLoop1], QOS1, CallMessageArrivedCallback)) != SUCCESS)
		{
			SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT subscribe Failed. idx = %d, result : %d", nLoop1, rc);
			break;
		}
	}

	CallSubscribedCallback(rc);

	return rc;
}

/**
 * @brief async subscribe
 * @param[in] topic The subscription topic, which may include wildcards.
 * @param[in] qos The requested quality of service for the subscription.
 * @return MQTTASYNC_SUCCESS if the subscription request is successful.
 */
int handleMQTTSubscribe(char* topic, int qos)
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTSubscribe()");

	if(g_pMQTTClient == NULL || !g_pMQTTClient->isConnected()|| topic == NULL || qos < (int)QOS0 || qos > (int)QOS2 )
	{
		SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT subscribe Failed. Parameter invalid.");
		return FAILURE;
	}

	int rc = SUCCESS;
	if((rc = g_pMQTTClient->subscribe(topic, (QoS)qos, CallMessageArrivedCallback)) != SUCCESS)
	{
		SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT subscribe Failed. topic : %s, qos : %d, result : %d", topic, qos, rc);
	}

	CallSubscribedCallback(rc);
    return rc;
}

/**
 * @brief publish message
 * @param[in] payload A pointer to the payload of the MQTT message.
 * @return MQTTASYNC_SUCCESS if the message is accepted for publication.
 */
int handleMQTTPublishMessage(char* payload)
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTPublishMessage()");

	if(g_pMQTTClient == NULL || !g_pMQTTClient->isConnected() || strlen(g_szPublishTopic) <= 0 || payload == NULL)
	{
		SKTDebugPrint(LOG_LEVEL_ERROR, "MQTTPublishMessage Failed. Parameter invalid.");
		SKTDebugPrint(LOG_LEVEL_ERROR, "mClient.isConnected : (%d)", g_pMQTTClient->isConnected());
		SKTDebugPrint(LOG_LEVEL_ERROR, "mPublishTopic : (%s)", g_szPublishTopic);
		SKTDebugPrint(LOG_LEVEL_ERROR, "payload : (%s)", payload);

        return FAILURE;
    }

	Message mMessage;

	mMessage.payload = (void*)payload;
	mMessage.payloadlen = strlen(payload);
	mMessage.qos = QOS1;
	mMessage.retained = false;
	mMessage.dup = false;

	SKTDebugPrint(LOG_LEVEL_DEBUG, "MQTT publish call.");
	SKTDebugPrint(LOG_LEVEL_DEBUG, "topic : %s", g_szPublishTopic);
	SKTDebugPrint(LOG_LEVEL_DEBUG, "payload : %s", mMessage.payload);
	SKTDebugPrint(LOG_LEVEL_DEBUG, "payloadlen : %d", mMessage.payloadlen);
	SKTDebugPrint(LOG_LEVEL_DEBUG, "qos : %d", mMessage.qos);
	SKTDebugPrint(LOG_LEVEL_DEBUG, "retained : %d", mMessage.retained);
	SKTDebugPrint(LOG_LEVEL_DEBUG, "dup : %d", mMessage.dup);

	int rc = SUCCESS;
	if((rc = g_pMQTTClient->publish(g_szPublishTopic, mMessage)) != SUCCESS)
	{
		SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT publish Failed. result : %d ", rc);
	}
	else
	{
		SKTDebugPrint(LOG_LEVEL_DEBUG, "MQTT publish Success.");
		CallMessageDeliveredCallback();
	}

	return rc;
}

int handleMQTTYield(unsigned long timeout_ms)
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTYield()");

	int rc = SUCCESS;
	if(g_pMQTTClient != NULL)
	{
		rc = g_pMQTTClient->yield(timeout_ms);
	}

	return rc;
}

/**
 * @brief is connected
 * @return true : connected <-> false
 */
int handleMQTTIsConnected()
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTIsConnected()");

	int rc = FAILURE;
	if(g_pMQTTClient != NULL)
	{
		rc = g_pMQTTClient->isConnected();
	}

	return rc;
}

/**
 * @brief disconnect mqtt
 */
int handleMQTTDisconnect()
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTDisconnect()");

	int rc = SUCCESS;
	if(g_pMQTTClient != NULL && g_pMQTTClient->isConnected())
	{
		rc = g_pMQTTClient->disconnect();

		g_pNetwork->disconnect();
	}

	CallDisconnectedCallback(rc);

	return rc;
}

/**
 * @brief destroy mqtt
 */
void handleMQTTDestroy()
{
	SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTDestroy()");

	if(g_pMQTTClient != NULL && g_pMQTTClient->isConnected())
	{
		handleMQTTDisconnect();
	}

	if(gContent) {
		if(gContent->data) {
			free(gContent->data);
			gContent->data = NULL;
		}
		free(gContent);
		gContent = NULL;
	}
	if(g_pMQTTClient != NULL)
	{
		delete g_pNetwork;
		delete g_pMQTTClient;
		g_pMQTTClient = NULL;
	}
}

}

#ifdef __cplusplus
}
#endif
