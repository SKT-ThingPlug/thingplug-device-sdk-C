#ifndef _NTP_CLIENT_H_
#define _NTP_CLIENT_H_

#include <stdint.h>

typedef struct _NTPPacket //See RFC 4330 for Simple NTP
{
	//LSb first
	unsigned mode : 3;
	unsigned vn : 3;
	unsigned li : 2;

	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;
	//32 bits header

	uint32_t rootDelay;
	uint32_t rootDispersion;
	uint32_t refId;

	uint32_t refTm_s;
	uint32_t refTm_f;
	uint32_t origTm_s;
	uint32_t origTm_f;
	uint32_t rxTm_s;
	uint32_t rxTm_f;
	uint32_t txTm_s;
	uint32_t txTm_f;
} NTPPacket;

int NTPSocket(void);
int setTime(void);
int get_npt_offset (void);

#endif /* _NTP_CLIENT_H_ */
