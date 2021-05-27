#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "config.h"
#include <chprintf.h>
#include <stdarg.h>

void debugInit(void);
int debug(const char *fmt, ...);

#ifdef DEBUG
#define DBG(X, ...)    debug(X, ##__VA_ARGS__)
#else 
#define DBG(...)
#endif 

#endif