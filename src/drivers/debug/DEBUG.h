#ifndef DEBUG_H_
#define DEBUG_H_

#include "stdio.h"

void log_debug( const char *file, const long line, const char *format, ... );
void log_info( const char *format, ... );
void log_print( const char *format, ... );
void DEBUG_Init( void );

void ShowDeviceInfo( void );

#if (defined DEBUG && defined LOG)
#define LOG_DEBUG(...) log_debug(__FILE__, __LINE__, __VA_ARGS__)

#define LOG_INFO(...)  log_info(__VA_ARGS__)

#define LOG_ASSERT(EXPR)                                                       \
if (!(EXPR))                                                                  \
{                                                                             \
    LOG_DEBUG("(%s) has assert failed at %s.\n", #EXPR, __FUNCTION__);         \
    while (1);                                                                \
}
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_ASSERT(...)
#endif

#endif
