//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_UTIL_H
#define DNSR_UTIL_H

#include <stdio.h>

extern char log_mask;

#define log_debug(args...) \
    if(log_mask & 1) \
    { \
        fprintf(stdout, "\x1b[33m[DEBUG]\x1b[36m %s:%d \x1b[0m", __FILE__, __LINE__); \
        fprintf(stdout, args); \
        fprintf(stdout, "\n"); \
    }

#define log_info(args...) \
    if(log_mask & 2) \
    { \
        fprintf(stdout, "\x1b[33m[INFO ]\x1b[36m %s:%d \x1b[0m", __FILE__, __LINE__); \
        fprintf(stdout, args); \
        fprintf(stdout, "\n"); \
    }

#define log_error(args...) \
    if(log_mask & 4) \
    { \
        fprintf(stderr, "\x1b[33m[ERROR]\x1b[36m %s:%d \x1b[0m", __FILE__, __LINE__); \
        fprintf(stderr, args); \
        fprintf(stderr, "\n"); \
    }

#define log_fatal(args...) \
    if(log_mask & 8) \
    { \
        fprintf(stderr, "\x1b[33m[FATAL]\x1b[36m %s:%d \x1b[0m", __FILE__, __LINE__); \
        fprintf(stderr, args); \
        fprintf(stderr, "\n"); \
    }

#endif //DNSR_UTIL_H
