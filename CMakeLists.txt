cmake_minimum_required(VERSION 3.17)
project(dnsr C)

set(CMAKE_C_STANDARD 11)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

add_executable(main
        main.c
        config_jar.h
        config_jar.c
        util.h
        dns_structure.h
        dns_conversion.c
        dns_conversion.h
        dns_client.c
        dns_client.h
        dns_server.c
        dns_server.h
        udp_pool.c
        udp_pool.h
        queue.c
        queue.h
        dns_print.c
        dns_print.h
        rbtree.c
        rbtree.h
        cache.c
        cache.h
        query_pool.c
        query_pool.h)
target_link_libraries(main uv curl)