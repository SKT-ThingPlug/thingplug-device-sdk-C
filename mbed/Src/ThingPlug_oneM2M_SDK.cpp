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

extern int SDKVerificationMain();

Serial pc(USBTX, USBRX);

/**
 * @brief main
 * @param[in] argc
 * @param[in] argv
 */
int main(int argc, char **argv) {
	pc.baud(115200);

//	SDKVerificationMain();
	MARun();
    return 0;
}

