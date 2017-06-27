
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

#include "sys/types.h"
#include "sys/sysinfo.h"

#include "SMA.h"

#define SMA_STRCMP(x,y) strncmp(x,y,strlen(y))

int dummyIdx = 0;

long long getFreeMem( ) {

   struct sysinfo memInfo;
   sysinfo (&memInfo);
   long long freeMem = memInfo.freeram*memInfo.mem_unit;

   return freeMem;
}

long long getTotalMem( ) {

   struct sysinfo memInfo;
   sysinfo (&memInfo);
   long long totalMem = memInfo.totalram * memInfo.mem_unit;

   return totalMem;
}

int getFreeMemMB( ) {
   int freeMemMB = getFreeMem()/1024/1024;
   return freeMemMB;
}

int getTotalMemMB( ) {

   int totalMemMB = getTotalMem()/1024/1024;
   return totalMemMB;

}


int SMAGetSensorIndex(char *sensorType)
{
    int idx = -1;
    if( SMA_STRCMP(sensorType,"batterystate") == 0 ) {
        idx = BATTERYSTATE_IDX;
    } else if( SMA_STRCMP(sensorType, "temperature") == 0 ) {
        idx = TEMP_IDX;
    } else if( SMA_STRCMP(sensorType, "humidity") == 0 ) {
        idx = HUMI_IDX;
    } else if( SMA_STRCMP(sensorType, "light") == 0 ) {
        idx = LIGHT_IDX;
    } else if( SMA_STRCMP(sensorType, "totalmem") == 0 ) {
        idx = SYS_TOTALMEM_IDX;
    } else if( SMA_STRCMP(sensorType, "freemem") == 0 ) {
        idx = SYS_FREEMEM_IDX;
    }
    return idx;
}

void SMAGetData(char *sensorType, char** output)
{
    int idx = SMAGetSensorIndex(sensorType);
    switch(idx) {
        case BATTERYSTATE_IDX:
            *output = strdup("1");
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
        case SYS_TOTALMEM_IDX:
            *output = (char *)malloc(5);
            sprintf(*output, "%4d", getTotalMemMB());
            break;
        case SYS_FREEMEM_IDX:
            *output = (char *)malloc(5);
            sprintf(*output, "%4d", getFreeMemMB());
            break;
        default:
			*output = NULL;
            break;
    }
	dummyIdx++;
	dummyIdx%=2;
}

