//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_CONFIG_JAR_H
#define DNSR_CONFIG_JAR_H

#include "util.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct _config_jar
{
    // 处理参数
    
    // 上一级 DNS 服务器 IP
    char *src_host;
    // 配置文件名
    char *config_filename;
    /**
     * 0: trace
     * 1: debug
     * 2: info
     * 3: error
     * 4: fatal
     */
    char *log_level;
}config_jar, *pconfig_jar;

/**
 * @brief pconfig_jar 构造函数
 *
 * @param argc
 * @param argv
 * @return pconfig_jar
 */
pconfig_jar new_config_jar(int argc, char *const *argv){
    
    new(jar, config_jar);
    
    print_log(TRACE, "parsing args...");
    int opt = -1;
    while (-1 != (opt = getopt(argc, argv, "c:s:"))) {
        switch (opt) {
            case 'c':
                printf("config file path: %s\n", optarg);
                jar->config_filename = strdup(optarg);
                break;
            case 's':
                printf("source DNS: %s\n", optarg);
                jar->src_host = strdup(optarg);
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

#endif //DNSR_CONFIG_JAR_H
