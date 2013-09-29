// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _REPORTING_H_
#define _REPORTING_H_
#include <cstdlib>

enum log_level
{
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

void log(log_level, const char*);
void logf(log_level, const char*, ...);
void log_own_buffer(log_level l, char* b);

#endif //_REPORTING_H_
