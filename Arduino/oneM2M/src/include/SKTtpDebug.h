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

#include "config.h"


typedef enum tagLogLevel {
	LOG_LEVEL_NONE =0,
	LOG_LEVEL_VERBOSE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARM,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL,
	LOG_LEVEL_MAX
} LOG_LEVEL_E;

typedef enum tagBooleanType {
	False = 0,
	True	
} BOOLEAN_TYPE_E;

#ifdef __cplusplus
extern "C" {
#endif


#if defined(SPT_DEBUG_ENABLE) || defined(DEBUG_ENABLE)

#ifdef ONEM2M_LINUX
	#include <stdio.h>
	extern void SKTtpDebugInit(BOOLEAN_TYPE_E enable, LOG_LEVEL_E level, FILE *stream);
	extern void SKTtpDebugPrintf(const char *filename, int lineno, LOG_LEVEL_E level, const char *format, ...);

	#define SKTDebugInit(enable, level, stream) 	SKTtpDebugInit(enable, level, stream)
	#define SKTDebugPrint(level, ...)             	SKTtpDebugPrintf(__FILE__, __LINE__, level, ## __VA_ARGS__)
#elif defined (ONEM2M_ARDUINO)
	extern void SKTtpDebugInit(BOOLEAN_TYPE_E enable, LOG_LEVEL_E level, void *stream);
	extern void SKTtpDebugPrintf(const char *filename, int lineno, LOG_LEVEL_E level, const char *format, ...);

	#define SKTDebugInit(enable, level, stream) 	SKTtpDebugInit((BOOLEAN_TYPE_E)enable, (LOG_LEVEL_E)level, (void*)stream)
	#define SKTDebugPrint(level, fmt, ...)			SKTtpDebugPrintf(__FILE__, __LINE__, (LOG_LEVEL_E)level, fmt, ## __VA_ARGS__)
#endif
	
#else
	#define SKTDebugInit(enable, level, stream)        ((void)0)
	#define SKTDebugPrint(...)                           ((void)0)
#endif

#ifdef __cplusplus
}
#endif


#endif /* _LOG_PRINT_H_ */
