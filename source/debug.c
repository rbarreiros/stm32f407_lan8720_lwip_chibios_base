#include "config.h"
#include "debug.h"
#include <ch.h>

#ifdef DEBUG
mutex_t gdbLock;
#endif

void debugInit(void)
{
#ifdef DEBUG 
    sdStart(DEBUG_SERIAL, NULL);
    chMtxObjectInit(&gdbLock); 
#endif
}

int debug(const char *fmt, ...)
{
#ifdef DEBUG
    va_list ap;
    int formatted_bytes;

    va_start(ap, fmt);
    chMtxLock(&gdbLock);
    formatted_bytes = chvprintf((BaseSequentialStream*)DEBUG_SERIAL, fmt, ap);
    chMtxUnlock(&gdbLock);    
    va_end(ap);

    return formatted_bytes;
#else
    (void)fmt;
    return 0;
#endif
}