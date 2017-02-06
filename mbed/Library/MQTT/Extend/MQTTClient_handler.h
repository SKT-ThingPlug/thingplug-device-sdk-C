#if !defined(MQTTCLIENT_HANDLER_H)
#define MQTTCLIENT_HANDLER_H

#include "oneM2MConfig.h"
#include "MQTTClient.h"
#ifdef IMPORT_TLS_LIB
#include "MQTTEthernetTLS.h"
#else
#include "MQTTEthernetExt.h"
#endif

namespace MQTT
{

class MQTTClient_handler
{
public:
	MQTTClient_handler(MQTTEthernetExt& network, unsigned int command_timeout_ms = 30000);
#ifdef IMPORT_TLS_LIB
	MQTTClient_handler(MQTTEthernetTLS& network, unsigned int command_timeout_ms = 30000);
#endif

	~MQTTClient_handler();

    typedef void (*messageHandler)(MessageData&);

	/** Set the default message handling callback - used for any message which does not match a subscription message handler
	 *  @param mh - pointer to the callback function
	 */
	void setDefaultMessageHandler(messageHandler mh);

	/** MQTT Connect - send an MQTT connect packet down the network and wait for a Connack
	 *  The nework object must be connected to the network endpoint before calling this
	 *  Default connect options are used
	 *  @return success code -
	 */
	int connect();

		/** MQTT Connect - send an MQTT connect packet down the network and wait for a Connack
	 *  The nework object must be connected to the network endpoint before calling this
	 *  @param options - connect options
	 *  @return success code -
	 */
	int connect(MQTTPacket_connectData& options);

	/** MQTT Publish - send an MQTT publish packet and wait for all acks to complete for all QoSs
	 *  @param topic - the topic to publish to
	 *  @param message - the message to send
	 *  @return success code -
	 */
	int publish(const char* topicName, Message& message);

	/** MQTT Publish - send an MQTT publish packet and wait for all acks to complete for all QoSs
	 *  @param topic - the topic to publish to
	 *  @param payload - the data to send
	 *  @param payloadlen - the length of the data
	 *  @param qos - the QoS to send the publish at
	 *  @param retained - whether the message should be retained
	 *  @return success code -
	 */
	int publish(const char* topicName, void* payload, size_t payloadlen, enum QoS qos = QOS0, bool retained = false);

	/** MQTT Publish - send an MQTT publish packet and wait for all acks to complete for all QoSs
	 *  @param topic - the topic to publish to
	 *  @param payload - the data to send
	 *  @param payloadlen - the length of the data
	 *  @param id - the packet id used - returned
	 *  @param qos - the QoS to send the publish at
	 *  @param retained - whether the message should be retained
	 *  @return success code -
	 */
	int publish(const char* topicName, void* payload, size_t payloadlen, unsigned short& id, enum QoS qos = QOS1, bool retained = false);

	/** MQTT Subscribe - send an MQTT subscribe packet and wait for the suback
	 *  @param topicFilter - a topic pattern which can include wildcards
	 *  @param qos - the MQTT QoS to subscribe at
	 *  @param mh - the callback function to be invoked when a message is received for this subscription
	 *  @return success code -
	 */
	int subscribe(const char* topicFilter, enum QoS qos, messageHandler mh);

	/** MQTT Unsubscribe - send an MQTT unsubscribe packet and wait for the unsuback
	 *  @param topicFilter - a topic pattern which can include wildcards
	 *  @return success code -
	 */
	int unsubscribe(const char* topicFilter);

	/** MQTT Disconnect - send an MQTT disconnect packet, and clean up any state
	 *  @return success code -
	 */
	int disconnect();

	/** A call to this API must be made within the keepAlive interval to keep the MQTT connection alive
	 *  yield can be called if no other MQTT operation is needed.  This will also allow messages to be
	 *  received.
	 *  @param timeout_ms the time to wait, in milliseconds
	 *  @return success code - on failure, this means the client has disconnected
	 */
	int yield(unsigned long timeout_ms = 1000L);

	/** Is the client connected?
	 *  @return flag - is the client connected or not?
	 */
	bool isConnected();

private:
	Client<MQTTEthernetExt, Countdown, MQTT_MAX_PACKET_SIZE, MQTT_MAX_HANDLER_CNT>*	m_pMQTTClient;
#ifdef IMPORT_TLS_LIB
	Client<MQTTEthernetTLS, Countdown, MQTT_MAX_PACKET_SIZE, MQTT_MAX_HANDLER_CNT>*	m_pMQTTClientTLS;
#endif

	bool			m_bUseTLS;
};

MQTTClient_handler::MQTTClient_handler(MQTTEthernetExt& network, unsigned int command_timeout_ms)
{
	m_bUseTLS = false;

	m_pMQTTClient = new Client<MQTTEthernetExt, Countdown, MQTT_MAX_PACKET_SIZE, MQTT_MAX_HANDLER_CNT>(network, command_timeout_ms);
#ifdef IMPORT_TLS_LIB
	m_pMQTTClientTLS = NULL;
#endif
}

#ifdef IMPORT_TLS_LIB
MQTTClient_handler::MQTTClient_handler(MQTTEthernetTLS& network, unsigned int command_timeout_ms)
{
	m_bUseTLS = true;

	m_pMQTTClient = NULL;
	m_pMQTTClientTLS = new Client<MQTTEthernetTLS, Countdown, MQTT_MAX_PACKET_SIZE, MQTT_MAX_HANDLER_CNT>((MQTTEthernetTLS&)network, command_timeout_ms);
}
#endif

MQTTClient_handler::~MQTTClient_handler()
{
	if(m_pMQTTClient != NULL)		{ delete m_pMQTTClient; }
#ifdef IMPORT_TLS_LIB
	if(m_pMQTTClientTLS != NULL)	{ delete m_pMQTTClientTLS; }
#endif
}

void MQTTClient_handler::setDefaultMessageHandler(messageHandler mh)
{
	if(!m_bUseTLS)	{ m_pMQTTClient->setDefaultMessageHandler(mh); }
#ifdef IMPORT_TLS_LIB
	else			{ m_pMQTTClientTLS->setDefaultMessageHandler(mh); }
#endif
}

int MQTTClient_handler::connect()
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->connect(); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->connect(); }
#endif
}

int MQTTClient_handler::connect(MQTTPacket_connectData& options)
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->connect(options); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->connect(options); }
#endif
}

int MQTTClient_handler::publish(const char* topicName, Message& message)
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->publish(topicName, message); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->publish(topicName, message); }
#endif
}

int MQTTClient_handler::publish(const char* topicName, void* payload, size_t payloadlen, enum QoS qos, bool retained)
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->publish(topicName, payload, payloadlen, qos, retained); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->publish(topicName, payload, payloadlen, qos, retained); }
#endif
}

int MQTTClient_handler::publish(const char* topicName, void* payload, size_t payloadlen, unsigned short& id, enum QoS qos, bool retained)
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->publish(topicName, payload, payloadlen, id, qos, retained); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->publish(topicName, payload, payloadlen, id, qos, retained); }
#endif
}

int MQTTClient_handler::subscribe(const char* topicFilter, enum QoS qos, messageHandler mh)
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->subscribe(topicFilter, qos, mh); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->subscribe(topicFilter, qos, mh); }
#endif
}

int MQTTClient_handler::unsubscribe(const char* topicFilter)
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->unsubscribe(topicFilter); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->unsubscribe(topicFilter); }
#endif
}

int MQTTClient_handler::disconnect()
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->disconnect(); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->disconnect(); }
#endif
}

int MQTTClient_handler::yield(unsigned long timeout_ms)
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->yield(timeout_ms); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->yield(timeout_ms); }
#endif
}

bool MQTTClient_handler::isConnected()
{
	if(!m_bUseTLS)	{ return m_pMQTTClient->isConnected(); }
#ifdef IMPORT_TLS_LIB
	else			{ return m_pMQTTClientTLS->isConnected(); }
#endif
}

}

#endif /* MQTTCLIENT_HANDLER_H */
