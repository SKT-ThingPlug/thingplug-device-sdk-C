/**
 * @file SKTtpDebug.h
 *
 * @brief Log print header file
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#ifndef _LOG_PRINT_H_
#define _LOG_PRINT_H_

#include "oneM2MConfig.h"

typedef enum tagLogLevel {
    LOG_LEVEL_NONE =0,
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_MAX
} LOG_LEVEL_E;

typedef enum tagBooleanType {
    False = 0,
    True
} BOOLEAN_TYPE_E;

#ifdef DEBUG_ENABLE
    #include <stdio.h>
    extern void SKTtpDebugInit(BOOLEAN_TYPE_E enable, LOG_LEVEL_E level);
    extern void SKTtpDebugPrintf( LOG_LEVEL_E level, char *str);

    #define SKTDebugInit(enable, level) SKTtpDebugInit(enable, level)
    #define SKTDebugPrint             SKTtpDebugPrintf
#else
    #define SKTDebugInit(enable, level)        ((void)0)
    #define SKTDebugPrint(...)                           ((void)0)
#endif

#endif /* _LOG_PRINT_H_ */
