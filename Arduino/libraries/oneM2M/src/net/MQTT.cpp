/**
 * @file MQTTClient.c
 *
 * @brief MQTT Client for Linux
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include <SPI.h>
#include <Ethernet.h>
#include "MQTT/IPStack.h"
#include "MQTT/Countdown.h"
#include "MQTT/MQTTClient.h"
#include "include/MQTT.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#define MQTT_MESSAGE_HANDLER_MAX_NUM	2
#define MQTT_PACKET_SIZE				1024
#define MQTT_RETRY_CNT 					10
#define MQTT_RETRY_INTERVAL				100


EthernetClient mEthernet; 
IPStack mIpstack(mEthernet);
MQTT::Client<IPStack, Countdown, MQTT_PACKET_SIZE, MQTT_MESSAGE_HANDLER_MAX_NUM> mClient = MQTT::Client<IPStack, Countdown, MQTT_PACKET_SIZE, MQTT_MESSAGE_HANDLER_MAX_NUM>(mIpstack);

static char mPublishTopic[128];
static char mSubscribeTopic[MQTT_MESSAGE_HANDLER_MAX_NUM][128];
static int mSubscribeTopicsize;

static tpMQTTConnectedCallback* mConnectedCallback;
static tpMQTTSubscribedCallback* mSubscribedCallback;
static tpMQTTDisconnectedCallback* mDisconnectedCallback;
static tpMQTTConnectionLostCallback* mConnectionLostCallback;
static tpMQTTMessageDeliveredCallback* mMessageDeliveredCallback;
static tpMQTTMessageArrivedCallback* mMessageArrivedCallback;

Content* gContent = NULL;

void OnConnect(void* context){
    if(mConnectedCallback) mConnectedCallback(MQTT::SUCCESS);
}

void OnConnectFailure(void* context){
    if(mConnectedCallback) mConnectedCallback(MQTT::FAILURE);
}

void OnSubscribe(void* context) {
    mSubscribeTopicsize--;    
	Serial.println("OnSubscribe");
    if(mSubscribedCallback && mSubscribeTopicsize == 0) mSubscribedCallback(MQTT::SUCCESS);
}

void OnSubscribeFailure(void* context) {
    if(mSubscribedCallback) mSubscribedCallback(MQTT::FAILURE);
}

void OnDisconnect(void* context){
    if(mDisconnectedCallback) mDisconnectedCallback(MQTT::SUCCESS);
}

void ConnectionLostCallback(void *context, char *cause) {
    if(mConnectionLostCallback) mConnectionLostCallback(cause);    
}

void MessageArrivedCallback(MQTT::MessageData& md)
{
	Serial.println("MessageArrivedCallback");
	MQTT::Message &message = md.message;

	if(message.dup)
		return;
	
    if(mMessageArrivedCallback) {
		mMessageArrivedCallback(md.topicName.lenstring.data, (char*)message.payload, message.payloadlen);
    }
	
}

void MessageDeliveredCallback(void *context) {
    if(mMessageDeliveredCallback) mMessageDeliveredCallback((int)0);
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
int MQTTSetCallbacks(tpMQTTConnectedCallback* cc, tpMQTTSubscribedCallback* sc, tpMQTTDisconnectedCallback* dc,
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac) {

    if(mac == NULL) {
        return -1;
    }
    
    mConnectedCallback = cc;
    mSubscribedCallback = sc;
    mDisconnectedCallback = dc;
    mConnectionLostCallback = clc;
    mMessageDeliveredCallback = mdc;
    mMessageArrivedCallback = mac;

    return 0;
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
 * @param[in] enabledCipherSuites cipher format. If this setting is ommitted, its default value will be "ALL".
 * @param[in] cleanSession if cleanSession=true, then the previous session information is cleared.
 * @param[in] clientID The client identifier(no longer 23 characters).
 * @return the return code of the connection response
 */
int MQTTCreate(char* host, int port, int keepalive, char* userName, char* password, 
    int enableServerCertAuth, char* subscribeTopic[], int subscribeTopicSize, char* publishTopic, char* enabledCipherSuites, int cleanSession, char* clientID) {
	Serial.println("MQTTCreate");
	long randNumber;		// for unique id

	char _clientID[32] = {0,};
	char clientID_prefix[] = "arduino_client_id_";
	int rc = 0; 
	uint8_t retry = 0;

	while(MQTT_RETRY_CNT > retry)
	{
		mIpstack.disconnect();
		rc = mIpstack.connect(host, port);
		if (rc == 1)  	// Ethernet Client Success : 1, Fail : 0
			break;

		Serial.print(retry);
		Serial.println(" << retry");
		retry++;
		delay(MQTT_RETRY_INTERVAL);
	}
	if(rc != 1)
	{
		Serial.print(host);
		Serial.print("<< host ");
		Serial.print(port);
		Serial.println(" << port");
		OnConnectFailure(&mClient);
		return MQTT::FAILURE;;
	}

	randomSeed(analogRead(0));
	randNumber = random(50000);

	Serial.print(randNumber);
	Serial.println(" << randNubmer");

	if(clientID == NULL)
	{
		sprintf(_clientID, "%s%d", clientID_prefix, (int)randNumber);
		clientID = _clientID;
		Serial.print(clientID);
		Serial.println(" << ClientID");
	}
	

	MQTTPacket_connectData conn_opts = MQTTPacket_connectData_initializer;  
	conn_opts.MQTTVersion = 3;
	conn_opts.clientID.cstring = clientID;
	conn_opts.keepAliveInterval = keepalive;
	if(userName)
		conn_opts.username.cstring = userName;
    if(password)
		conn_opts.password.cstring = password;
	conn_opts.cleansession = cleanSession;

	retry = 0;
	while(MQTT_RETRY_CNT > retry)
	{
		rc = mClient.connect(conn_opts);
		if (rc == 0)  // success 0
			break;

		Serial.print(retry);
		Serial.println(" << MQTT Retry");
		retry++;
		delay(MQTT_RETRY_INTERVAL);		
	}

 	if (rc != 0)
	{
		Serial.print(rc);
		Serial.println(" << MQTT connect failed!");
		OnConnectFailure(&mClient);
		return MQTT::FAILURE;
	}
	
	OnConnect(&mClient);

    memset(mPublishTopic, 0, sizeof(mPublishTopic));
    memcpy(mPublishTopic, publishTopic, strlen(publishTopic));
	
    mSubscribeTopicsize = subscribeTopicSize;
    for(int i = 0; i < subscribeTopicSize; i++) {
        memset(mSubscribeTopic[i], 0, sizeof(mSubscribeTopic[i]));
        strncpy(mSubscribeTopic[i], subscribeTopic[i], strlen(subscribeTopic[i]));

		Serial.print("MQTT Request subscibe topic : ");
		Serial.println(mSubscribeTopic[i]);
		rc = mClient.subscribe(mSubscribeTopic[i], MQTT::QOS1, MessageArrivedCallback);  
		if (rc != 0)  
		{	  
			Serial.print("MQTT Request subscibe Failed : ");
			Serial.println(mSubscribeTopic[i]);
			OnSubscribeFailure(&mClient);
			return MQTT::FAILURE;
		}
		else
		{
			OnSubscribe(&mClient);
		}
    }


    return MQTT::SUCCESS;
}

/**
 * @brief async subscribe
 * @param[in] topic The subscription topic, which may include wildcards.
 * @param[in] qos The requested quality of service for the subscription.
 * @return MQTTASYNC_SUCCESS if the subscription request is successful.
 */
int MQTTSubscribe(char* topic, int qos) {
	Serial.println("MQTTSubscribe");
	
	int rc = MQTT::SUCCESS;

	if(!mClient.isConnected()|| topic == NULL || qos < (int)MQTT::QOS0 || qos > (int)MQTT::QOS2 )
	{
		Serial.println("MQTTSubscribe failed[1]");
		return MQTT::FAILURE;
	}
	
	rc = mClient.subscribe(topic, (MQTT::QoS)qos, MessageArrivedCallback);  
	if(rc != 0)
	{
		Serial.println("MQTTSubscribe failed[2]");
		OnSubscribeFailure(&mClient);
		return MQTT::FAILURE;
	}

	OnSubscribe(&mClient);
    return rc;
}

/**
 * @brief publish message
 * @param[in] payload A pointer to the payload of the MQTT message.
 * @return MQTTASYNC_SUCCESS if the message is accepted for publication.
 */
int MQTTPublishMessage(char* payload) {
	Serial.println("MQTTPublishMessage");

    if(!mClient.isConnected() || strlen(mPublishTopic) <= 0 || payload == NULL) 
	{
		Serial.print("payload : ");
		Serial.println((char*)payload);
		
        return MQTT::FAILURE;
    }

	MQTT::Message mMessage;

	mMessage.payload = (void*)payload;
	mMessage.payloadlen = strlen((char*)payload);
	mMessage.qos = MQTT::QOS1;
	mMessage.retained = false;
	mMessage.dup = false;

	char buf[128];
	snprintf(buf,128,"payloadlen = %d",mMessage.payloadlen);
	Serial.println(buf);
	Serial.println("MQTT publish topic-->");
	Serial.println((char*)mPublishTopic);
	Serial.println("<--");
	Serial.println("payload->");
	Serial.println((char*)mMessage.payload);
	Serial.println("<--");
	
	int rc = mClient.publish(mPublishTopic, mMessage);
	if(rc != MQTT::SUCCESS)
	{
		Serial.println("MQTT publish failed[1]");
	}
	else
	{
		Serial.println("MQTT publish Success");
		MessageDeliveredCallback(&mClient);
	}

	return rc;
}

/**
 * @brief disconnect mqtt
 */
int MQTTDisconnect() {
	Serial.println("MQTTDisconnect");
    
    int rc = MQTT::SUCCESS;
	if(mClient.isConnected())
	{
		rc = mClient.disconnect();
		if( rc == MQTT::SUCCESS)
			OnDisconnect(&mClient);

		mIpstack.disconnect();
	}
    return (int)rc;
}

/**
 * @brief destroy mqtt
 */
void MQTTDestroy() {
    Serial.println("MQTTDestory");
    if(gContent) {
        if(gContent->data) {
            free(gContent->data);
            gContent->data = NULL;
        }
        free(gContent);
        gContent = NULL;
    } 
    // disconnect when connected.
    if(mClient.isConnected()) {
        MQTTDisconnect();
	}
}

/**
 * @brief is connected
 * @return true : connected <-> false
 */
int MQTTIsConnected() {
    return  mClient.isConnected();
}


int MQTTyield(unsigned long timeout_ms)
{
	return mClient.yield(timeout_ms);
}


#ifdef __cplusplus
}
#endif
