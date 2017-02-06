
#include <Arduino.h>

#include <stdio.h>
#include <string.h>

#include "StreamWrapper.h"

extern "C"
{
	
	size_t Stream_print_str(void* stream, const char* str)
	{
		size_t rc;
		if(stream != NULL)
		{
			rc = ((Stream*)stream)->print(str);
			((Stream*)stream)->flush();
		}
		else
		{
			rc = Serial.print(str);
			Serial.flush();
		}

		return rc;
	}

	
	size_t Stream_print(void* stream, char *fmt, ...)
	{
		char *buf = NULL;
		int rc = 0;

		buf = (char*)malloc(STREAM_BUF_MAX_SIZE * sizeof(char));
		memset(buf, 0x0, STREAM_BUF_MAX_SIZE * sizeof(char));
		va_list ap;

		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		va_end(ap);

		rc = Stream_print_str(stream, buf);
		if(buf) free(buf);
		
		return rc; 
	}
}

