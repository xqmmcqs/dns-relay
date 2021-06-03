cmake_minimum_required(VERSION 3.17)
project(dnsr C)

set(CMAKE_C_STANDARD 11)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

add_executable(main
        src/main.c
        include/util.h
        src/config_jar.c
        include/config_jar.h
        include/dns_structure.h
        src/dns_conversion.c
        include/dns_conversion.h
        src/dns_client.c
        include/dns_client.h
        src/dns_server.c
        include/dns_server.h
        src/index_pool.c
        include/index_pool.h
        src/queue.c
        include/queue.h
        src/dns_print.c
        include/dns_print.h
        src/rbtree.c
        include/rbtree.h
        src/cache.c
        include/cache.h
        src/query_pool.c
        include/query_pool.h)
target_link_libraries(main uv)