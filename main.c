#include <string.h>
#include "runtime.h"
#include "arg_parser.h"
#include "log.h"

int main(int argc, char* const* argv) {

  print_log(TRACE, "start.");
  arg_parse(argc, argv);
  print_log(INFO, "\nconfig_filename = %s", config_filename);
  // TODO: 监听服务端口 53
  return 0;
}
