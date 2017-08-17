/**
 * @file SKTtpDebug.c
 *
 * @brief Log print
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include "SKTtpDebug.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE*           gSKTtpDebugStream   = NULL;
BOOLEAN_TYPE_E  gSKTtpDebugEnable   = False;
LOG_LEVEL_E     gSKTtpDebugLogLevel = (LOG_LEVEL_E)LOG_LEVEL_NONE;

/*
 * SKTtpDebugLogLevelString
 */
void SKTtpDebugLogLevelString(LOG_LEVEL_E level, char *buffer)
{
	switch(level) {
		case LOG_LEVEL_VERBOSE: {
			memcpy(buffer, "VERBOSE", strlen("VERBOSE"));
		}
		break;

		case LOG_LEVEL_DEBUG: {
			memcpy(buffer, "DEBUG", strlen("DEBUG"));
		}
		break;

		case LOG_LEVEL_INFO: {
			memcpy(buffer, "INFO", strlen("INFO"));
		}
		break;

		case LOG_LEVEL_WARN: {
			memcpy(buffer, "WARN", strlen("WARN"));
		}
		break;

		case LOG_LEVEL_ERROR: {
			memcpy(buffer, "ERROR", strlen("ERROR"));
		}
		break;
		default: {
			memcpy(buffer, "NONE", strlen("NONE"));
		}
		break;
	}
}

/*
 * DebugInit
 */
void SKTtpDebugInit(BOOLEAN_TYPE_E enable, LOG_LEVEL_E level, FILE *stream)
{
	gSKTtpDebugEnable   = enable;
	gSKTtpDebugLogLevel = level;
	if(stream == NULL) {
		gSKTtpDebugStream = stderr;
	} else {
		gSKTtpDebugStream = stream;
	}
}

/*
 * TimeToString
 */
char* TimeToString(struct tm *t) {
  static char s[20];

  sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
              t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
              t->tm_hour, t->tm_min, t->tm_sec
          );

  return s;
}

/*
 * DebugPrint
 */
void SKTtpDebugPrintf(const char *filename, int lineno, LOG_LEVEL_E level, const char *format, ...)
{
    if(format == NULL) {
        return;
    }
    
	if(gSKTtpDebugEnable == False ) {
		return;
	}

	if(gSKTtpDebugLogLevel == LOG_LEVEL_NONE) {
		return;
	}

	if(gSKTtpDebugStream == NULL) {
		return;
	}

	if(gSKTtpDebugLogLevel > level) {
		return;
	}

	char stringBuffer[30];
	memset(stringBuffer, 0, 30);
	SKTtpDebugLogLevelString(level, stringBuffer);

    
    struct tm *t;
    time_t timer;
    
    timer = time(NULL);
    timer += 32400;
    t = localtime(&timer);

	fprintf(gSKTtpDebugStream, "[%s] [%s] [%d] [%s]: ", TimeToString(t), filename, lineno, stringBuffer);

	va_list argp;
	va_start(argp, format);
	vfprintf(gSKTtpDebugStream, format, argp);
	va_end(argp);
	fputs("\r\n", gSKTtpDebugStream);
	fflush(gSKTtpDebugStream);
}
