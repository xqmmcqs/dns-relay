#if !defined(__LOG_H__)
#define __LOG_H__

#include <stdio.h>

#define TRACE "trace"
#define DEBUG "debug"
#define INFO "info"
#define ERROR "error"
#define FATAL "fatal"

#define print_log(level, ...) \
    fprintf(stdout,"\x1b[33m[%s]\x1b[36m %s:%d \x1b[0m", level, __FILE__, __LINE__); \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n"); 


#endif  // __LOG_H__
