#if !defined(__ARG_PARSER_H__)
#define __ARG_PARSER_H__

#include <stdio.h>
#include <string.h>
#include <string.h>  // memcpy
#include <unistd.h>
#include "log.h"

// TODO: 把修改全局变量的方式改成传参
int arg_parse(int argc, char* const* argv) {
  print_log(TRACE, "parsing args...");
  int opt = -1;
  while (-1 != (opt = getopt(argc, argv, "c:s:"))) {
    switch (opt) {
      case 'c':
        printf("config file: %s\n", optarg);
        config_filename = strdup(optarg);
        break;
      case 's':
        printf("source DNS: %s\n", optarg);
        src_host = strdup(optarg);
        break;
      case ':':
        printf("missing argument.\n");
        break;
      case '?':
        printf("unknown option: %c\n", optopt);
        break;
      default:
        break;
    }
  }
  print_log(TRACE, "finished parsing.");
}

#endif  // __ARG_PARSER_H__
