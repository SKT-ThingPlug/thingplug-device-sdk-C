#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TTVBuilder.h"


#define HTONS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
#define NTOHS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))

#define HTONL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
                          ((((unsigned long)(n) & 0xFF00)) << 8) | \
                          ((((unsigned long)(n) & 0xFF0000)) >> 8) | \
                          ((((unsigned long)(n) & 0xFF000000)) >> 24))

#define NTOHL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
                          ((((unsigned long)(n) & 0xFF00)) << 8) | \
                          ((((unsigned long)(n) & 0xFF0000)) >> 8) | \
                          ((((unsigned long)(n) & 0xFF000000)) >> 24))


#define TTV_DEF(_a,_b,_c) void _b ## _convert_func(char *origin, char* output, int len);
#include "TTVDefinitions.h"
#undef TTV_DEF

#define TTV_DEF(_a,_b,_c) void _b ## _ret_convert_func(char *origin, char* output, int len);
#include "TTVDefinitions.h"
#undef TTV_DEF

typedef struct _ttv_handle_t{
    void (* convert_func)(char* origin, char* output, int len);
    void (* ret_convert_func)(char* origin, char* output, int len);
    short len;
}ttv_handle_t;

#define TTV_DEF(_a,_b,_c) [_a] = { _b ## _convert_func, _b ## _ret_convert_func ,_c },
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

int TTVFormattingValue(char datatype, char *in, char *out)
{
    int idx = (int)datatype;
    int len = ttv_handle[idx].len;

    ttv_handle[idx].ret_convert_func(in, out, len);

    return 0;
}

int TTVGetDatatypeLen(char ttv_datatype)
{
    return ttv_handle[(int)ttv_datatype].len;
}


// ***************************************************************
// List of convert functions (value to string)
// ***************************************************************
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
    value = (short)HTONS(value);
    memcpy(output ,&value, len);
}


void ushort_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    unsigned short value;
    value = (unsigned short)atoi(origin);
    // output example : 22370 (integer)
    value = (unsigned short)HTONS(value);
    memcpy(output ,&value, len);
}

void int_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    int value;
    value = (int)atoi(origin);
    value = (int)HTONL(value);
    // output example : 22370 (integer)
    memcpy(output ,&value, len);
}


void uint_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    unsigned int value;
    value = (unsigned int)atoi(origin);
    // output example : 22370 (integer)
    value = (int)HTONL(value);
    memcpy(output ,&value, len);
}

void long_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    long value;
    value = (long)atoi(origin);
    value = (long)HTONL(value);
    // output example : 22370 (integer)
    memcpy(output ,&value, len);
}


void ulong_convert_func(char* origin, char* output, int len)
{
    // data example :  22377(string)
    unsigned long value;
    value = (unsigned long)atoi(origin);
    value = (unsigned long)HTONL(value);
    // output example : 22370 (integer)
    memcpy(output ,&value, len);
}

void float_convert_func(char* origin, char* output, int len)
{
    // data example :  22377.0(string)
    float value;
    int tmp;
    value = (float)atof(origin);
    tmp = (int)HTONL(*(int*)&value);
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
    unsigned int *value;
    int tmp;
    value = (unsigned int *)origin;
    tmp = (int)HTONL(*value);
    memcpy(output ,&tmp, len);
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

// ***************************************************************
// List of convert functions (string to value)
// ***************************************************************
void string_to_byte(char *origin, char* out, int len)
{
    int i,tmp;
    char one[3];

    for( i = 0; i < len; i += 2) {
        one[0] = origin[i];
        one[1] = origin[i+1];
        one[2] = '\0';
        sscanf(one, "%x", &tmp);
        out[i/2] = (unsigned char)tmp;
    }
}

void boolean_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
}

void char_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
}

void uchar_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
}

void short_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
    short tmp;
    tmp = NTOHS( *(short*)output);
    memcpy(output,&tmp,len/2);
}

void ushort_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
    unsigned short tmp;
    tmp = NTOHS( *(unsigned short*)output);
    memcpy(output,&tmp,len/2);
}

void int_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
    int tmp;
    tmp= NTOHL( *(int *)output);
    memcpy(output,&tmp,len/2);
}

void uint_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
    unsigned int tmp;
    tmp= NTOHL( *(unsigned int *)output);
    memcpy(output,&tmp,len/2);
}

void long_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
    long tmp;
    tmp= NTOHL( *(long *)output);
    memcpy(output,&tmp,len/2);
}

void ulong_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
    unsigned long tmp;
    tmp= NTOHL( *(unsigned long *)output);
    memcpy(output,&tmp,len/2);
}

void float_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
    unsigned long tmp;
    tmp= NTOHL( *(unsigned long *)output);
    memcpy(output,&tmp,len/2);
}

void double_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin, output, len);
}

void mac_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin,output,len);
}

void time_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin,output,len);
}

void latitude_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin,output,len);
}

void longitude_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin,output,len);
}

void string_ret_convert_func(char* origin, char* output, int len)
{
    string_to_byte(origin,output,len);
}

void precisetime_ret_convert_func(char* origin, char* output, int len)
{
    return;
}
