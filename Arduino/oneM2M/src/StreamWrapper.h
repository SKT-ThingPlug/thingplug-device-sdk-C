#pragma once

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STREAM_BUF_MAX_SIZE		1024

size_t Stream_print(void* stream, char *fmt, ...);
size_t Stream_print_str(void* stream, const char* str);

#ifdef __cplusplus
}
#endif
