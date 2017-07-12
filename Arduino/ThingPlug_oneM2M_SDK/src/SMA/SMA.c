
/**
 * @file SMA.c
 *
 * @brief SensorManagementAgent Process
 *
 * Copyright (C) 2016. SKT, All Rights Reserved.
 * Written 2016,by SKT
 */

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "SMA.h"
#include <StreamWrapper.h>

#ifdef USE_REAL_SENSOR
#include "rgb_lcd.h"
#endif

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
	} else if( SMA_STRCMP(sensorType, "proximity") == 0 ) {
		idx = PROX_IDX;
    }
    return idx;
}

unsigned long GetProximityValue() {
#ifdef USE_REAL_SENSOR
    unsigned long echo = 0;
    unsigned long value = 0;
    pinMode(PROX_PIN, OUTPUT); // Switch signalpin to output
    digitalWrite(PROX_PIN, LOW); // Send low pulse 
    delayMicroseconds(2); // Wait for 2 microseconds
    digitalWrite(PROX_PIN, HIGH); // Send high pulse
    delayMicroseconds(5); // Wait for 5 microseconds
    digitalWrite(PROX_PIN, LOW); // Holdoff
    pinMode(PROX_PIN, INPUT); // Switch signalpin to input
    digitalWrite(PROX_PIN, HIGH); // Turn on pullup resistor
    // please note that pulseIn has a 1sec timeout, which may
    // not be desirable. Depending on your sensor specs, you
    // can likely bound the time like this -- marcmerlin
    // echo = pulseIn(distPin, HIGH, 38000)
    echo = pulseIn(PROX_PIN, HIGH); //Listen for echo
    value = (echo/2) / 29.1;
    return value;
#else
	return 1;
#endif
}

void SMAGetData(char *sensorType, char** output)
{
    int idx = SMAGetSensorIndex(sensorType);
    switch(idx) {
        case MOTION_IDX:
        {
			if( dummyIdx == 0 ) {
                *output = strdup("1");
            } else {
                *output = strdup("0");
            }
            break;
		}
        case TEMP_IDX:
		{
#ifdef USE_REAL_SENSOR
			char temp[10] = "";
            int base = analogRead(TEMP_PIN);
            float R = 1024.0/((float)base)-1.0; 
            int B = 4275;
            R = 100000.0 * R;
            float r = 1.0/(log(R/100000.0)/B+1/298.15)-273.15;            
            snprintf(temp, sizeof(temp), "%d", (int)(r*100/2)); // half value when 5V
            *output = strdup(temp);
#else
            if( dummyIdx == 0 ) {
                *output = strdup("26.26");
                dummyIdx = 1;
            } else {
                *output = strdup("25.25");
            }
#endif
            break;
        }
		case HUMI_IDX:
		{
#ifdef USE_REAL_SENSOR
            char light[10] = "";
            int r = analogRead(LIGHT_PIN)/2;
            snprintf(light, sizeof(light), "%d", r/2); // half value when 5V 
            *output = strdup(light);
#else
            if( dummyIdx == 0 ) {
                *output = strdup("48");
            } else {
                *output = strdup("44");
            }
#endif
            break;
        }
		case LIGHT_IDX:
		{
            if( dummyIdx == 0 ) {
                *output = strdup("278");
            } else {
                *output = strdup("267");
            }
            break;
        }
		case PROX_IDX:
        {
            char prox[10] = "";
            unsigned long r = GetProximityValue();
            snprintf(prox, sizeof(prox), "%ld", r);
            *output = strdup(prox);
            break;
        }
        default:
			*output = NULL;
            break;
    }
	dummyIdx++;
	dummyIdx%=2;
}

void SMASetLED(int pin, int flag)
{
	if( flag == 0){
		digitalWrite(pin, LOW);
	} else {
		digitalWrite(pin, HIGH);
	}
}

#ifdef USE_LCD
rgb_lcd lcd;

void SMAInitLCD(int x, int y)
{
	lcd.begin(x, y); //16, 2
}
void SMASetLCDRGB(int R, int G, int B)
{
	lcd.setRGB(R, G, B);
}
void SMASetLCD(int x, int y, char *str, int clear_flag)
{
	if( clear_flag == 1 ) {
		lcd.clear();
	}
	lcd.setCursor(x, y);
	lcd.print(str);
}
#endif

