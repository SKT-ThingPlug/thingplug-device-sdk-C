#if !defined(MQTTETHERNETTLS_H)
#define MQTTETHERNETTLS_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_DEBUG_C)
#include "mbedtls/compat-1.3.h"
#include "mbedtls/debug.h"
#define DEBUG_LEVEL						3
#endif

#include "MQTTEthernetExt.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ctr_drbg.h"

typedef struct{
#if defined (MBEDTLS_ENTROPY_C)
	mbedtls_entropy_context* entropy;
#endif
	mbedtls_ctr_drbg_context* ctr_drbg;
	mbedtls_ssl_context* ssl;
	mbedtls_ssl_config* conf;
	mbedtls_x509_crt* cacert;
}wiz_ssl_context;

class MQTTEthernetTLS : public MQTTEthernetExt
{
public:
	MQTTEthernetTLS();

	int		connect(char* hostname, int port, int timeout=1000);

	int		read(unsigned char* buffer, int len, int timeout);

	int		write(unsigned char* buffer, int len, int timeout);

	int		disconnect();

private:
	wiz_ssl_context		m_sslContext;
};

#endif
