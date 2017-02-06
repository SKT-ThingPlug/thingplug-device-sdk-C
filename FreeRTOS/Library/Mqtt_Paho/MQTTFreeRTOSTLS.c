#include "MQTTFreeRTOSTLS.h"

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

#include "mbedtls/ssl.h"
#include "mbedtls/ctr_drbg.h"

#define RECV_TIMEOUT_VALUE			2000

typedef struct{
#if defined (MBEDTLS_ENTROPY_C)
	mbedtls_entropy_context* entropy;
#endif
	mbedtls_ctr_drbg_context* ctr_drbg;
	mbedtls_ssl_context* ssl;
	mbedtls_ssl_config* conf;
	mbedtls_x509_crt* cacert;
}wiz_ssl_context;

wiz_ssl_context		g_sslContext;

int SSLSendCB(void* ctx, const unsigned char* buf, unsigned int len)
{
	return FreeRTOS_write((Network*)ctx, (unsigned char*)buf, len, 3000);
}

int SSLRecvCB(void* ctx, unsigned char* buf, unsigned int len)
{
	return FreeRTOS_read((Network*)ctx, (unsigned char*)buf, len, 3000);
}

int SSLRecvTimeOutCB(void* ctx, unsigned char* buf, size_t len, uint32_t timeout)
{
	return FreeRTOS_read((Network*)ctx, (unsigned char*)buf, len, timeout);
}

#if defined (MBEDTLS_DEBUG_C)
void SSLDebugCB(void* ctx, int level, const char* file, int line, const char* str)
{
	if(level < DEBUG_LEVEL)
	{
		printf("%s\r\n", str);
	}
}
#endif

int SSLRandomCB(void* p_rng, unsigned char* output, size_t output_len)
{
	if(output_len <= 0)
	{
		return (1);
	}
	for(unsigned int nLoop1 = 0 ; nLoop1 < output_len ; nLoop1++)
	{
		*output++ = rand() % 0xff;
	}
	srand(rand());

	return (0);
}

int EthernetTLS_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	mbedtls_ssl_conf_read_timeout(g_sslContext.conf, timeout_ms);
	return mbedtls_ssl_read(g_sslContext.ssl, buffer, len);
}

int EthernetTLS_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	return mbedtls_ssl_write(g_sslContext.ssl, buffer, len);
}

void EthernetTLS_disconnect(Network* n)
{
	FreeRTOS_disconnect(n);
}

void NetworkInitTLS(Network* n)
{
	NetworkInit(n);
	n->mqttread = EthernetTLS_read;
	n->mqttwrite = EthernetTLS_write;
	n->disconnect = EthernetTLS_disconnect;

	int ret = 1;
//#if defined (MBEDTLS_ERROR_C)
//	char error_buf[100];
//#endif

#if defined (MBEDTLS_DEBUG_C)
	debug_set_threshold(DEBUG_LEVEL);
#endif

	/*
	Initialize session data
	*/
#if defined (MBEDTLS_ENTROPY_C)
	g_sslContext.entropy = malloc(sizeof(mbedtls_entropy_context));
	mbedtls_entropy_init(g_sslContext.entropy);
#endif
	g_sslContext.ctr_drbg = (mbedtls_ctr_drbg_context*)malloc(sizeof(mbedtls_ctr_drbg_context));
	g_sslContext.ssl = (mbedtls_ssl_context*)malloc(sizeof(mbedtls_ssl_context));
	g_sslContext.conf = (mbedtls_ssl_config*)malloc(sizeof(mbedtls_ssl_config));
	g_sslContext.cacert = (mbedtls_x509_crt*)malloc(sizeof(mbedtls_x509_crt));

	mbedtls_ctr_drbg_init(g_sslContext.ctr_drbg);
	mbedtls_x509_crt_init(g_sslContext.cacert);
	mbedtls_ssl_init(g_sslContext.ssl);
	mbedtls_ssl_config_init(g_sslContext.conf);

	/*
	Initialize certificates
	*/
#if defined (MBEDTLS_X509_CRT_PARSE_C)
#if defined (MBEDTLS_DEBUG_C)
	printf(" Loading the CA root certificate \r\n");
#endif
	mbedtls_ssl_config_defaults((g_sslContext.conf),
								MBEDTLS_SSL_IS_CLIENT,
								MBEDTLS_SSL_TRANSPORT_STREAM,
								MBEDTLS_SSL_PRESET_DEFAULT);
	mbedtls_ssl_setup((g_sslContext.ssl), (g_sslContext.conf));
	//mbedtls_ssl_set_timer_cb(g_sslContext.ssl, NULL, SSLFSetTimerCB, SSLFGetTimerCB);
	//mbedtls_ssl_set_hostname(g_sslContext.ssl, HOST_NAME);
#if defined (MBEDTLS_CERTS_C)
	ret = mbedtls_x509_crt_parse((g_sslContext.cacert), (unsigned char *)CERTIFICATE, strlen(CERTIFICATE));
#else
	ret = 1;
#if defined (MBEDTLS_DEBUG_C)
	printf("SSL_CERTS_C not define .\r\n");
#endif
#endif
#endif
	if(ret < 0)
	{
#if defined (MBEDTLS_CERTS_C)
		printf("x509_crt_parse failed.%x \r\n", ret);
#endif
		return;
	}

	/*
	set ssl session param
	*/
	mbedtls_ssl_conf_ca_chain(g_sslContext.conf, g_sslContext.cacert, NULL);
	mbedtls_ssl_conf_endpoint(g_sslContext.conf, MBEDTLS_SSL_IS_CLIENT); 		//set the current communication method is SSL Client
//	mbedtls_ssl_conf_authmode(g_sslContext.conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_authmode(g_sslContext.conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(g_sslContext.conf, SSLRandomCB, g_sslContext.ctr_drbg);
	mbedtls_ssl_conf_read_timeout(g_sslContext.conf, RECV_TIMEOUT_VALUE);
#if defined (MBEDTLS_DEBUG_C)
	mbedtls_ssl_conf_dbg(g_sslContext.conf, SSLDebugCB, stdout);
#endif
	mbedtls_ssl_set_bio(g_sslContext.ssl, (void*)n, SSLSendCB, SSLRecvCB, SSLRecvTimeOutCB);		 //set client's socket send and receive functions
}

int NetworkConnectTLS(Network* n, char* addr, int port)
{
	int ret;

	ret = NetworkConnect(n, addr, port);
	if(ret != 0)
	{
		return ret;
	}

	uint32_t flags;
//#if defined(MBEDTLS_ERROR_C)
//    unsigned char error_buf[100];
//    memset(error_buf, 0, 100);
//#endif
#if defined(WIZINTERFACE_DEBUG)
	printf( "  . Performing the SSL/TLS handshake..." );
#endif

	mbedtls_ssl_set_hostname(g_sslContext.ssl, addr);
//	mbedtls_ssl_set_hostname(g_sslContext.ssl, mysock->get_address());
//	mbedtls_ssl_set_hostname(g_sslContext.ssl, "");

	while( ( ret = mbedtls_ssl_handshake( g_sslContext.ssl ) ) != 0 )
	{
		printf("mbedtls_ssl_handshake() returned %d\r\n", ret);
		if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
		{
//#if defined(MBEDTLS_ERROR_C)
//            mbedtls_strerror( ret, (char *) error_buf, 100 );
//            printf( " failed\n\r  ! mbedtls_ssl_handshake returned %d: %s\n\r", ret, error_buf );
//#endif
			return( -1 );
		}
	}
#if defined(WIZINTERFACE_DEBUG)
	printf( " ok\n\r    [ Ciphersuite is %s ]\n\r", mbedtls_ssl_get_ciphersuite( g_sslContext.ssl ) );
	printf( "  . Verifying peer X.509 certificate..." );
#endif
	/* In real life, we probably want to bail out when ret != 0 */
	if( ( flags = mbedtls_ssl_get_verify_result( g_sslContext.ssl ) ) != 0 )
	{
		//char vrfy_buf[512];
#if defined(WIZINTERFACE_DEBUG)
		printf( " failed.\n\r" );
#endif
		//mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

		//printf( "%s\n\r", vrfy_buf );

		return flags;
	}
	else
		printf( " ok.\n\r" );

	return 0;
}

void NetworkDisconnectTLS(Network* n)
{
	mbedtls_ssl_free(g_sslContext.ssl);
	free(g_sslContext.ssl);
	mbedtls_ssl_config_free(g_sslContext.conf);
	free(g_sslContext.conf);
	mbedtls_ctr_drbg_free(g_sslContext.ctr_drbg);
	free(g_sslContext.ctr_drbg);

#if defined (MBEDTLS_ENTROPY_C)
	mbedtls_entropy_free(g_sslContext.entropy);
	free(g_sslContext.entropy);
#endif
#if defined(MBEDTLS_X509_CRT_PARSE_C)
	mbedtls_x509_crt_free(g_sslContext.cacert);
	free(g_sslContext.cacert);
#endif

#ifdef USE_ETHERNET_LIB
	NetworkDisconnect(n);
#endif
}
