
/**
 * @file SRA.c
 *
 * @brief ServiceReadyAgent Process
 *
 * Copyright (C) 2016. SKT, All Rights Reserved.
 * Written 2016,by SKT
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SRA.h"
#include "GetTime.h"
#include "../SMA/SMA.h"
#include "TLVMaker.h"

typedef enum _tlv_sensor_list {
    TLV_BATTERY_LEVEL,
    TLV_DATE,
    TLV_TIME,
    TLV_TEMPERATURE,
    TLV_LIGHT,
    TLV_HUMIDITY,
    TLV_GPS_NORTH,
    TLV_GPS_EAST,
    TLV_BUZZER,
    TLV_PH,
    TLV_LED,
    TLV_NOISE,
    TLV_PRESSURE,
    TLV_WINDVANE,
    TLV_DUST,
    TLV_CO2,
    TLV_DOOR,
    TLV_MOTION,
    TLV_ONOFF,
    TLV_PERCENT,
    TLV_COMPLEX,
    TLV_STRING,
    TLV_NUMBER,
    TLV_CUSTOM,
    TLV_UNDEFINED,
    TLV_SENSOR_MAX
} TLV_SENSOR_T;


typedef struct _tlv_policy {
    char mTag;
    char mLength;
} TLV_POLICY_T;


static TLV_POLICY_T gPolicy[TLV_SENSOR_MAX] = 
{
    {0x01, 0x01}, //TLV_BATTERY_LEVEL
    {0x02, 0x03}, //TLV_DATE
    {0x03, 0x03}, //TLV_TIME
    {0x23, 0x02}, //TLV_TEMPERATURE
    {0x25, 0x02}, //TLV_LIGHT
    {0x26, 0x01}, //TLV_HUMIDITY
    {0x20, 0x03}, //TLV_GPS_NORTH
    {0x21, 0x03}, //TLV_GPS_EAST 
    {0x27, 0x01}, //TLV_BUZZER
    {0x2D, 0x02}, //TLV_PH
    {0x28, 0x01}, //TLV_LED
    {0x29, 0x01}, //TLV_NOISE
    {0x24, 0x02}, //TLV_PRESSURE
    {0x2A, 0x02}, //TLV_WINDVANE
    {0x2B, 0x02}, //TLV_DUST
    {0x2C, 0x02}, //TLV_CO2
    {0x2E, 0x01}, //TLV_DOOR
    {0x2F, 0x01}, //TLV_MOTION
    {0x30, 0x01}, //TLV_ONOFF
    {0x31, 0x01}, //TLV_PERCENT
    {0x32, 0x00}, //TLV_COMPLEX
    {0x33, 0x00}, //TLV_STRING
    {0x34, 0x00}, //TLV_NUMBER
    {0x35, 0x00}, //TLV_CUSTOM
    {0x36, 0x00}, //TLV_UNDEFINED
};

int GetPolicyIndex(char *sensorType)
{

    int sensorIndex = -1;

    if( strncmp( sensorType, "battery", sizeof("battery")) == 0 ) {
        sensorIndex = TLV_BATTERY_LEVEL;
    } else if( strncmp( sensorType, "temperature", sizeof("temperature")) == 0 ) {
        sensorIndex = TLV_TEMPERATURE;
    } else if( strncmp( sensorType, "light", sizeof("light")) == 0 ) {
        sensorIndex = TLV_LIGHT;
    } else if( strncmp( sensorType, "humidity", sizeof("humidity")) == 0 ) {
        sensorIndex = TLV_HUMIDITY;
    } else if( strncmp( sensorType, "gps", sizeof("gps")) == 0 ) {
        sensorIndex = TLV_GPS_NORTH;
    } else if( strncmp( sensorType, "buzzer", sizeof("buzzer")) == 0 ) {
        sensorIndex = TLV_BUZZER;
    } else if( strncmp( sensorType, "led", sizeof("led")) == 0 ) {
        sensorIndex = TLV_LED;
    } else if( strncmp( sensorType, "noise", sizeof("noise")) == 0 ) {
        sensorIndex = TLV_NOISE;
    } else if( strncmp( sensorType, "pressure", sizeof("pressure")) == 0 ) {
        sensorIndex = TLV_PRESSURE;
    } else if( strncmp( sensorType, "windVane", sizeof("windVane")) == 0 ) {
        sensorIndex = TLV_WINDVANE;
    } else if( strncmp( sensorType, "dust", sizeof("dust")) == 0 ) {
        sensorIndex = TLV_DUST;
    } else if( strncmp( sensorType, "co2", sizeof("co2")) == 0 ) {
        sensorIndex = TLV_CO2;
    } else if( strncmp( sensorType, "ph", sizeof("ph")) == 0 ) {
        sensorIndex = TLV_PH;
    } else if( strncmp( sensorType, "door", sizeof("door")) == 0 ) {
        sensorIndex = TLV_DOOR;
    } else if( strncmp( sensorType, "motion", sizeof("motion")) == 0 ) {
        sensorIndex = TLV_MOTION;
    } else if( strncmp( sensorType, "onoff", sizeof("onoff")) == 0 ) {
        sensorIndex = TLV_ONOFF;
    } else if( strncmp( sensorType, "percent", sizeof("percent")) == 0 ) {
        sensorIndex = TLV_PERCENT;
    } else if( strncmp( sensorType, "complex", sizeof("complex")) == 0 ) {
        sensorIndex = TLV_COMPLEX;
    } else if( strncmp( sensorType, "string", sizeof("string")) == 0 ) {
        sensorIndex = TLV_STRING;
    } else if( strncmp( sensorType, "number", sizeof("number")) == 0 ) {
        sensorIndex = TLV_NUMBER;
    } else if( strncmp( sensorType, "custom", sizeof("custom")) == 0 ) {
        sensorIndex = TLV_CUSTOM;
    } else if( strncmp( sensorType, "undefined", sizeof("undefined")) == 0 ) {
        sensorIndex = TLV_UNDEFINED;
    } 
	return sensorIndex;
}

void SRASetValueToTLV(char *sensorType, char* value)
{
    int idx = SMAGetSensorIndex(sensorType);
    int intTemp;
    int i;
    switch(idx) {
        case MOTION_IDX:
            if(strncmp(value,"0", 1) == 0) {
                value[0] = 0x0;
            } else {
                value[0] = 0x1;
            }
            break;
        case TEMP_IDX:
            for( i = 0; i < strlen(value); i++ ) {
                if(value[i] == '.') break;
            }
            intTemp = atoi(value);
            intTemp = intTemp*100 + atoi(&value[i+1]);
            value[0] = (intTemp >> 8) & 0xff;
            value[1] = intTemp & 0xff;
            break;
        case HUMI_IDX:
            intTemp = atoi(value);
            value[0] = intTemp & 0xff;
            break;
        case LIGHT_IDX:
            intTemp = atoi(value);
            value[0] = intTemp & 0xff;
            break;
        default:
            break;
    }
}

void SRADataConvert(int event, void *data)
{
    switch(event) {
        case IOT_RAW_TO_TLV:
			{
                void *tlv;
                void *chunk;
                raw2tlv *new = (raw2tlv *) data;
                new->tlv = NULL;
                int idx = GetPolicyIndex(new->type);
                SRASetValueToTLV(new->type, new->value);
                tlv = CreateTLV(); 
                chunk = CreateChunk();
                SetChunk(gPolicy[idx].mTag,gPolicy[idx].mLength,new->value,chunk);
                PushChunk(chunk,tlv);
                MakeTLV(tlv);
                new->tlv = (char *)calloc(1, GetTLVSize(tlv)+1);
                memcpy(new->tlv , GetTLV(tlv), GetTLVSize(tlv));
                DestroyTLV(tlv);
        	}
            break;
        case IOT_GET_TIME_TLV:
            {
                void *tlv;
                void *chunk;
                char local_date[3];
                char local_time[3];
                timetlv *new =  (timetlv *) data;
                local_date[0] = (char)(getYear() - 2000)&0xff;
                local_date[1] = (char)getMonth()&0xff;
                local_date[2] = (char)getDay()&0xff;
                local_time[0] = (char)getHour()&0xff;
                local_time[1] = (char)getMinute()&0xff;
                local_time[2] = (char)getSecond()&0xff;
                tlv = CreateTLV(); 
                chunk = CreateChunk();
                SetChunk(gPolicy[TLV_DATE].mTag,gPolicy[TLV_DATE].mLength,local_date,chunk);
                PushChunk(chunk,tlv);
                chunk = CreateChunk();
                SetChunk(gPolicy[TLV_TIME].mTag,gPolicy[TLV_TIME].mLength,local_time,chunk);
                PushChunk(chunk,tlv);
                MakeTLV(tlv);
                new->tlv = (char *)calloc(1, GetTLVSize(tlv)+1);
                memcpy(new->tlv , GetTLV(tlv), GetTLVSize(tlv));
                DestroyTLV(tlv);
            }
            break;
        case IOT_GET_GPS_TLV:
            {
                void *tlv;
                void *chunk;
                gpstlv *new =  (gpstlv *) data;
                tlv = CreateTLV(); 
                chunk = CreateChunk();
                SetChunk(gPolicy[TLV_GPS_NORTH].mTag,gPolicy[TLV_GPS_NORTH].mLength,new->north,chunk);
                PushChunk(chunk,tlv);
                chunk = CreateChunk();
                SetChunk(gPolicy[TLV_GPS_EAST].mTag,gPolicy[TLV_GPS_EAST].mLength,new->east,tlv);
                PushChunk(chunk,tlv);
                MakeTLV(tlv);
                new->tlv = (char *)calloc(1, GetTLVSize(tlv)+1);
                memcpy(new->tlv , GetTLV(tlv), GetTLVSize(tlv));
                DestroyTLV(tlv);
            }
            break;
        default:
            break;
    }
}

