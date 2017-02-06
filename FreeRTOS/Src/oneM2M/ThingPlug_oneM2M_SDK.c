/**
 * @file ThingPlug_oneM2M_SDK.c
 *
 * @brief Simple ThingPlug_oneM2M_SDK application
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include "MA.h"

extern int SDKVerificationMain();

/**
 * @brief main
 * @param[in] argc
 * @param[in] argv
 */
int ThingPlug_oneM2M_SDK(int argc, char **argv) {
//	SDKVerificationMain();
	MARun();
    return 0;
}

