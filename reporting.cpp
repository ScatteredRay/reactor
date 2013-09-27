#include "reporting.h"
#include "system.h"

#include <cstdio>
#include <cstdarg>

void log(log_level, const char* str)
{
    fputs(str, stderr);

    put_debug_string(str);
}

void logf(log_level level, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char* buffer = new char[len + 1];

    va_start(args, fmt);
    vsnprintf(buffer, len + 1, fmt, args);
    va_end(args);

    log(level, buffer);
    delete[] buffer;
}

void log_own_buffer(log_level l, char* b)
{
    log(l, b);
    free(b);
}
