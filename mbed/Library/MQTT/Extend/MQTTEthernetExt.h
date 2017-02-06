#if !defined(MQTTETHERNETEXT_H)
#define MQTTETHERNETEXT_H

#include "MQTTmbed.h"
#include "EthernetInterface.h"
#include "MQTTSocketExt.h"

class MQTTEthernetExt : public MQTTSocketExt
{
public:    
	MQTTEthernetExt()
    {
    	mysock = new TCPSocketConnection();

        eth.init();                          // Use DHCP
        eth.connect();
    }
    
    ~MQTTEthernetExt()
    {
    	delete mysock;
    }

    EthernetInterface& getEth()
    {
        return eth;
    }
    
    void reconnect()
    {
        eth.connect();  // nothing I've tried actually works to reconnect 
    }
    
private:

    EthernetInterface eth;
    
};


#endif
