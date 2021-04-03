cmake_minimum_required(VERSION 3.17)
project(dnsr C)

set(CMAKE_C_STANDARD 11)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)
add_executable(main main.c config_jar.h util.h dns_structure.h dns_convesion.c dns_convesion.h dns_server.c dns_server.h req_handler.c req_handler.h)
target_link_libraries(main -luv)