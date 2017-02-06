#if !defined(ETHERNETINTERFACE_H)
#define ETHERNETINTERFACE_H

#include "WIZnetInterface.h"

class EthernetInterface
{
public:
	EthernetInterface();
	~EthernetInterface();

	int		init();

	int		connect();

	char*	getIPAddress();
	char*	getNetworkMask();
	char*	getGateway();
	char*	getMACAddress();

private:
	WIZnetInterface		m_eth;
};

#endif
