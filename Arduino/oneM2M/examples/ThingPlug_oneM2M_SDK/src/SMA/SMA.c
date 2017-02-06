
/**
 * @file SMA.c
 *
 * @brief SensorManagementAgent Process
 *
 * Copyright (C) 2016. SKT, All Rights Reserved.
 * Written 2016,by SKT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SMA.h"
#include <StreamWrapper.h>

#define SMA_STRCMP(x,y) strncmp(x,y,strlen(y))

int dummyIdx = 0;

int SMAGetSensorIndex(char *sensorType)
{
    int idx = -1;
    if( SMA_STRCMP(sensorType,"motion") == 0 ) {
        idx = MOTION_IDX;
    } else if( SMA_STRCMP(sensorType, "temperature") == 0 ) {
        idx = TEMP_IDX;
    } else if( SMA_STRCMP(sensorType, "humidity") == 0 ) {
        idx = HUMI_IDX;
    } else if( SMA_STRCMP(sensorType, "light") == 0 ) {
        idx = LIGHT_IDX;
    }
    return idx;
}

void SMAGetData(char *sensorType, char** output)
{
    int idx = SMAGetSensorIndex(sensorType);
    switch(idx) {
        case MOTION_IDX:
            if( dummyIdx == 0 ) {
                *output = strdup("1");
            } else {
                *output = strdup("0");
            }
            break;
        case TEMP_IDX:
            if( dummyIdx == 0 ) {
                *output = strdup("26.26");
                dummyIdx = 1;
            } else {
                *output = strdup("25.25");
            }
            break;
        case HUMI_IDX:
            if( dummyIdx == 0 ) {
                *output = strdup("48");
            } else {
                *output = strdup("44");
            }
            break;
        case LIGHT_IDX:
            if( dummyIdx == 0 ) {
                *output = strdup("278");
            } else {
                *output = strdup("267");
            }
            break;
        default:
			*output = NULL;
            break;
    }
	dummyIdx++;
	dummyIdx%=2;
    Stream_print(NULL,"SMAGetData  = <%s>\n", *output);
}

