#include "EthernetInterface.h"

#include "oneM2MConfig.h"

EthernetInterface::EthernetInterface()
: m_eth(D11, D12, D13, D10, D9)		// mosi, miso, sclk, cs, reset
{
}

EthernetInterface::~EthernetInterface()
{
}

int EthernetInterface::init()
{
	unsigned char mac[] = MAC_ADDRESS;

	int rc = m_eth.init(mac);                          // Use DHCP
	if(rc != 0)
	{
		error("Ethernet init failed. result = %d\r\n", rc);
	}

	return rc;
}

int EthernetInterface::connect()
{
    int rc = m_eth.connect();
    if(rc != 0)
    	error("ethernet connect : %s\r\n", (rc==0)?"Success":"Fail");
    
    return rc;
}

char* EthernetInterface::getIPAddress()
{
   	return m_eth.getIPAddress();
}

char* EthernetInterface::getNetworkMask()
{
   	return m_eth.getNetworkMask();
}

char* EthernetInterface::getGateway()
{
   	return m_eth.getGateway();
}

char* EthernetInterface::getMACAddress()
{
   	return m_eth.getMACAddress();
}
