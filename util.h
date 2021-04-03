//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_UTIL_H
#define DNSR_UTIL_H

#include <stdio.h>

#define TRACE "trace"
#define DEBUG "debug"
#define INFO "info"
#define ERROR "error"
#define FATAL "fatal"

#define print_log(level, ...)                                            \
  fprintf(stdout, "\x1b[33m[%s]\x1b[36m %s:%d \x1b[0m", level, __FILE__, \
          __LINE__);                                                     \
  fprintf(stdout, __VA_ARGS__);                                          \
  fprintf(stdout, "\n");

#define new(instance_name, struct_name)\
struct_name *instance_name = (struct_name *)malloc(sizeof(struct_name));\
memset(instance_name, 0x0, sizeof(struct_name));

#endif //DNSR_UTIL_H
