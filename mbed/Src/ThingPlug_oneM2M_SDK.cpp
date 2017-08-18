/**
 * @file ThingPlug_oneM2M_SDK.c
 *
 * @brief Simple ThingPlug_oneM2M_SDK application
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include <mbed.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "MA.h"

#ifdef __cplusplus
}
#endif

Serial pc(USBTX, USBRX);

/**
 * @brief main
 * @param[in] argc
 * @param[in] argv
 */
int main(int argc, char **argv) {
	pc.baud(115200);
	fprintf(stdout, " MBED START! \r\n");
	while(1)
	{
		wait(2.0);
		MARun();
	}
    return 0;
}

