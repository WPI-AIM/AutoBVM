#ifndef UVENT_LOGGING_H
#define UVENT_LOGGING_H

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <variant.h>
#include "src/misc/lv_printf.h"

inline void serial_printf(const char* str, ...)
{
    char buf[256];
    va_list args;
    va_start(args, str);
    lv_vsnprintf(buf, 127, str, args);
    va_end(args);
    Serial.print(buf);
}

#endif//UVENT_LOGGING_H
