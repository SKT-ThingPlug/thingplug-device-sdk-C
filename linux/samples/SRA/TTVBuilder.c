
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "TTVBuilder.h"


#define TTV_DEF(_a,_b,_c) void _b ## _convert_func(char *origin, char* output, int len);
#include "TTVDefinitions.h"
#undef TTV_DEF

typedef struct _ttv_handle_t{
	void (* convert_func)(char* origin, char* output, int len);
	short len;
}ttv_handle_t;

#define TTV_DEF(_a,_b,_c) [_a] = { _b ## _convert_func, _c },
static ttv_handle_t ttv_handle[] = {
#include "TTVDefinitions.h"
};
#undef TTV_DEF

int TTVConvertData(char type, char datatype, char *in_buf, char **out_buf)
{

    int idx = (int)datatype;
    int len = ttv_handle[idx].len/2;
    *out_buf = (char*)calloc(1, len);
    if( *out_buf == NULL ) return -1;
    ttv_handle[idx].convert_func(in_buf, *out_buf, len);
    return len;
}


// List of convert functions
void boolean_convert_func(char* origin, char* output, int len)
{
    char value;
    value = (char) atoi(origin);
    memcpy(output ,&value, len); 
}

void char_convert_func(char* origin, char* output, int len)
{
    char value;
    value = (char) atoi(origin);
    memcpy(output ,&value, len); 
}

void uchar_convert_func(char* origin, char* output, int len)
{
    unsigned char value;
    value = (unsigned char) atoi(origin);
    memcpy(output ,&value, len); 
}

void short_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    short value;
    value = (short)atoi(origin);
    // output example : 22370 (integer)
    value = (short)htons(value);
    memcpy(output ,&value, len); 
}


void ushort_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    unsigned short value;
    value = (unsigned short)atoi(origin);
    // output example : 22370 (integer)
    value = (unsigned short)htons(value);
    memcpy(output ,&value, len); 
}

void int_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    int value;
    value = (int)atoi(origin);
    value = (int)htonl(value);
    // output example : 22370 (integer)
    memcpy(output ,&value, len); 
}


void uint_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    unsigned int value;
    value = (unsigned int)atoi(origin);
    // output example : 22370 (integer)
    value = (int)htonl(value);
    memcpy(output ,&value, len); 
}

void long_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    long value;
    value = (long)atoi(origin);
    value = (long)htonl(value);
    // output example : 22370 (integer)
    memcpy(output ,&value, len); 
}


void ulong_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    unsigned long value;
    value = (unsigned long)atoi(origin);
    value = (unsigned long)htonl(value);
    // output example : 22370 (integer)
    memcpy(output ,&value, len); 
}

void float_convert_func(char* origin, char* output, int len)
{
    // data example :  22377.0(string)
    float value;
    int tmp;
    value = (float)atof(origin);
    tmp = (int)htonl(*(int*)&value);
    // output example : 22370.0 (float)
    memcpy(output ,&tmp, len); 
}

void double_convert_func(char* origin, char* output, int len)
{
    // data example :  22377.0(string)
    double value;
    value = (double)atof(origin);
    // output example : 22370.0 (float)
    memcpy(output ,&value, len); 
}

void mac_convert_func(char* origin, char* output, int len)
{
    memcpy(output ,origin, len); 
}

void time_convert_func(char* origin, char* output, int len)
{
    memcpy(output ,origin, len); 
}

void latitude_convert_func(char* origin, char* output, int len)
{
    memcpy(output ,origin, len); 
}

void longitude_convert_func(char* origin, char* output, int len)
{
    memcpy(output ,origin, len); 
}

void string_convert_func(char* origin, char* output, int len)
{
    memcpy(output ,origin, len); 
}

void precisetime_convert_func(char* origin, char* output, int len)
{
	return;
}


