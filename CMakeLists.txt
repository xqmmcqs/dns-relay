cmake_minimum_required(VERSION 3.17)
project(dnsr C)

set(CMAKE_C_STANDARD 11)

link_directories(/usr/local/lib)
include_directories(/usr/local/include ./inc)
add_executable(main main.c)
target_link_libraries(main -luv)