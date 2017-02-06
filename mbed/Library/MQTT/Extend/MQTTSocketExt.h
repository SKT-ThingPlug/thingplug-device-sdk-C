#if !defined(MQTTSOCKETEXT_H)
#define MQTTSOCKETEXT_H

#include "MQTTmbed.h"
#include "TCPSocketConnection.h"

class MQTTSocketExt
{
public:    
    int connect(char* hostname, int port, int timeout=1000)
    {
        mysock->set_blocking(false, timeout);    // 1 second Timeout
        return mysock->connect(hostname, port);
    }

    int read(unsigned char* buffer, int len, int timeout)
    {
    	mysock->set_blocking(false, timeout);
        return mysock->receive((char*)buffer, len);
    }
    
    int write(unsigned char* buffer, int len, int timeout)
    {
    	mysock->set_blocking(false, timeout);
        return mysock->send((char*)buffer, len);
    }
    
    int disconnect()
    {
        return mysock->close();
    }
    
protected:

    TCPSocketConnection* mysock;
    
};



#endif
