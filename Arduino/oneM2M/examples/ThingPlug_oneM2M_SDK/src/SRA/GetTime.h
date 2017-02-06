
/**
 * @file getTime.h
 *
 * @brief Arduino get Time API header
 *
 * Copyright (C) 2016. SPTek,All Rights Reserved.
 * Written 2016,by SPTek 
 */


#ifndef _GET_TIME_H
#define _GET_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

void setNtpTime();
int getHour();
int getMinute();
int getSecond();
int getYear();
int getMonth();
int getDay();
void digitalClockDisplay();


#ifdef __cplusplus
}
#endif
#endif//_GET_TIME_H

