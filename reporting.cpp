#include "reporting.h"
#include <cstdio>

void log(log_level, const char* str)
{
    fputs(str, stdout);
}

void log_own_buffer(log_level l, char* b)
{
    log(l, b);
    free(b);
}
