#include "MQTTEthernetTLS.h"

#define RECV_TIMEOUT_VALUE			2000

//static uint32_t TimerReference;
//static uint32_t fTimeOutValue;
//static uint32_t iTimeOutValue;
//
///*
// * \param p_timer  parameter (context) shared by timer callback
// * \param f_set_timer   set timer callback
// *                 Accepts an intermediate and a final delay in milliseconcs
// *                 If the final delay is 0, cancels the running timer.
// * \param f_get_timer   get timer callback. Must return:
// *                 -1 if cancelled
// *                 0 if none of the delays is expired
// *                 1 if the intermediate delay only is expired
// *                 2 if the final delay is expired
// */
//void SSLFSetTimerCB(void *pTimer, uint32_t iTimeOut, uint32_t fTimeOut)
//{
//	if(!fTimeOut)
//	{
//		fTimeOutValue = 0;
//		fTimeOutValue = 0;
//		iTimeOutValue = 0;
//	}
//	else
//	{
//		TimerReference = HAL_GetTick();
//		fTimeOutValue = fTimeOut;
//		iTimeOutValue = iTimeOut;
//	}
//}
//
//int SSLFGetTimerCB(void *pTimer)
//{
//  	uint32_t CurrentTick = HAL_GetTick();
//	uint8_t TimerExpired = (((CurrentTick-TimerReference) > fTimeOutValue)?2:0)|(((CurrentTick-TimerReference) > iTimeOutValue)?1:0);
//	if(fTimeOutValue == 0) return -1;
//	switch(TimerExpired)
//	{
//	case 0://There is no timer expired.
//	  return 0;
//	  break;
//	case 1://Only iTimer is Expired.
//	  return 1;
//	  break;
//	case 2://Only fTimer is Expired. --> Error
//	  iTimeOutValue = 0;
//	  fTimeOutValue = 0;
//	  return -1;
//	  break;
//	case 3://iTimer and fTimer is expired.
//	  return 2;
//	  break;
//	default:
//	  return -1;
//	}
//}

int SSLSendCB(void* ctx, const unsigned char* buf, unsigned int len)
{
	return ((MQTTSocketExt*)ctx)->write((unsigned char*)buf, len, 3000);
}

int SSLRecvCB(void* ctx, unsigned char* buf, unsigned int len)
{
	return ((MQTTSocketExt*)ctx)->read((unsigned char*)buf, len, 3000);
}

int SSLRecvTimeOutCB(void* ctx, unsigned char* buf, size_t len, uint32_t timeout)
{
	return ((MQTTSocketExt*)ctx)->read((unsigned char*)buf, len, timeout);
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

MQTTEthernetTLS::MQTTEthernetTLS()
{
	int ret = 1;
//#if defined (MBEDTLS_ERROR_C)
//	char error_buf[100];
//#endif
	if( !isSocketReady ) {
		return;
	}
#if defined (MBEDTLS_DEBUG_C)
	debug_set_threshold(DEBUG_LEVEL);
#endif

	/*
	Initialize session data
	*/
#if defined (MBEDTLS_ENTROPY_C)
	m_sslContext.entropy = malloc(sizeof(mbedtls_entropy_context));
	mbedtls_entropy_init(m_sslContext.entropy);
#endif
	m_sslContext.ctr_drbg = (mbedtls_ctr_drbg_context*)malloc(sizeof(mbedtls_ctr_drbg_context));
	m_sslContext.ssl = (mbedtls_ssl_context*)malloc(sizeof(mbedtls_ssl_context));
	m_sslContext.conf = (mbedtls_ssl_config*)malloc(sizeof(mbedtls_ssl_config));
	m_sslContext.cacert = (mbedtls_x509_crt*)malloc(sizeof(mbedtls_x509_crt));

	mbedtls_ctr_drbg_init(m_sslContext.ctr_drbg);
	mbedtls_x509_crt_init(m_sslContext.cacert);
	mbedtls_ssl_init(m_sslContext.ssl);
	mbedtls_ssl_config_init(m_sslContext.conf);

	/*
	Initialize certificates
	*/
#if defined (MBEDTLS_X509_CRT_PARSE_C)
#if defined (MBEDTLS_DEBUG_C)
	printf(" Loading the CA root certificate \r\n");
#endif
	mbedtls_ssl_config_defaults((m_sslContext.conf),
								MBEDTLS_SSL_IS_CLIENT,
								MBEDTLS_SSL_TRANSPORT_STREAM,
								MBEDTLS_SSL_PRESET_DEFAULT);
	mbedtls_ssl_setup((m_sslContext.ssl), (m_sslContext.conf));
	//mbedtls_ssl_set_timer_cb(m_sslContext.ssl, NULL, SSLFSetTimerCB, SSLFGetTimerCB);
	//mbedtls_ssl_set_hostname(m_sslContext.ssl, HOST_NAME);
#if defined (MBEDTLS_CERTS_C)
	ret = mbedtls_x509_crt_parse((m_sslContext.cacert), (unsigned char *)CERTIFICATE, strlen(CERTIFICATE));
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
	mbedtls_ssl_conf_ca_chain(m_sslContext.conf, m_sslContext.cacert, NULL);
	mbedtls_ssl_conf_endpoint(m_sslContext.conf, MBEDTLS_SSL_IS_CLIENT); 		//set the current communication method is SSL Client
//	mbedtls_ssl_conf_authmode(m_sslContext.conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_authmode(m_sslContext.conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(m_sslContext.conf, SSLRandomCB, m_sslContext.ctr_drbg);
	mbedtls_ssl_conf_read_timeout(m_sslContext.conf, RECV_TIMEOUT_VALUE);
#if defined (MBEDTLS_DEBUG_C)
	mbedtls_ssl_conf_dbg(m_sslContext.conf, SSLDebugCB, stdout);
#endif
	mbedtls_ssl_set_bio(m_sslContext.ssl, (void*)this, SSLSendCB, SSLRecvCB, SSLRecvTimeOutCB);		 //set client's socket send and receive functions
}

int MQTTEthernetTLS::connect(char* hostname, int port, int timeout)
{
	int ret;

	ret = MQTTSocketExt::connect(hostname, port, timeout);
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

//	mbedtls_ssl_set_hostname(m_sslContext.ssl, hostname);
	mbedtls_ssl_set_hostname(m_sslContext.ssl, mysock->get_address());
//	mbedtls_ssl_set_hostname(m_sslContext.ssl, "");

	while( ( ret = mbedtls_ssl_handshake( m_sslContext.ssl ) ) != 0 )
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
	printf( " ok\n\r    [ Ciphersuite is %s ]\n\r", mbedtls_ssl_get_ciphersuite( m_sslContext.ssl ) );
	printf( "  . Verifying peer X.509 certificate..." );
#endif
	/* In real life, we probably want to bail out when ret != 0 */
	if( ( flags = mbedtls_ssl_get_verify_result( m_sslContext.ssl ) ) != 0 )
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

int MQTTEthernetTLS::read(unsigned char* buffer, int len, int timeout)
{
	mbedtls_ssl_conf_read_timeout(m_sslContext.conf, timeout);
	return mbedtls_ssl_read(m_sslContext.ssl, buffer, len);
}

int MQTTEthernetTLS::write(unsigned char* buffer, int len, int timeout)
{
	return mbedtls_ssl_write(m_sslContext.ssl, buffer, len);
}

int MQTTEthernetTLS::disconnect()
{
	mbedtls_ssl_free(m_sslContext.ssl);
	free(m_sslContext.ssl);
	mbedtls_ssl_config_free(m_sslContext.conf);
	free(m_sslContext.conf);
	mbedtls_ctr_drbg_free(m_sslContext.ctr_drbg);
	free(m_sslContext.ctr_drbg);

#if defined (MBEDTLS_ENTROPY_C)
	mbedtls_entropy_free(m_sslContext.entropy);
	free(m_sslContext.entropy);
#endif
#if defined(MBEDTLS_X509_CRT_PARSE_C)
	mbedtls_x509_crt_free(m_sslContext.cacert);
	free(m_sslContext.cacert);
#endif

	MQTTSocketExt::disconnect();

	return 0;
}
