#include <stdarg.h>

static int DBG = 0;

void printer(char *fmt, ...) {
    if( ! DBG ) return;
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

