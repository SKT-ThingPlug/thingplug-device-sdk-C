#if !defined(MQTTETHERNETEXT_H)
#define MQTTETHERNETEXT_H

#include "MQTTmbed.h"
#include "EthernetInterface.h"
#include "MQTTSocketExt.h"

class MQTTEthernetExt : public MQTTSocketExt
{
public:    
	int isSocketReady;
	MQTTEthernetExt()
    {
    	int rc;
    	mysock = new TCPSocketConnection();

        rc = eth.init();                          // Use DHCP
        if(rc != 0) {
        	delete mysock;
        	isSocketReady = 0;
        	return;
        }
        rc = eth.connect();
        if(rc != 0) {
        	delete mysock;
        	isSocketReady = 0;
        	return;
        }
        isSocketReady = 1;
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
