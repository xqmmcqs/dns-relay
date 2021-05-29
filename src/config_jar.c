/**
 * @file      config_jar.c
 * @brief     
 * @author    xqmmcqs
 * @date      2021/5/29
 * @copyright GNU General Public License, version 3 (GPL-3.0)
*/

#include "../include/config_jar.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uv.h>

#include "../include/util.h"

char * REMOTE_HOST = "10.3.9.4";
int LOG_MASK = 15;
int CLIENT_PORT = 0;
char * HOSTS_PATH = "./hosts.txt";
char * LOG_PATH;

static void parse_error(char * error_msg)
{
    log_fatal("命令行参数有误 %s", error_msg);
    exit(1);
}

void init_config(int argc, char * const * argv)
{
    log_debug("解析命令行参数");
    argc--;
    argv++;
    if (argc == 1 && strcmp(*argv, "--help") == 0)
    {
        exit(0);
    }
    int i = 0;
    while (i < argc)
    {
        char * field = argv[i];
        if (field[0] != '-' && field[1] != '-')parse_error("参数标志必须以--开头");
        field += 2;
        if (strcmp(field, "remote_host") == 0)
        {
            if (i + 1 == argc)parse_error("缺少参数值");
            char * dest = (char *) malloc(10 * sizeof(char));
            if (!dest)
            {
                log_fatal("分配内存失败");
                exit(1);
            }
            if (uv_inet_pton(AF_INET, argv[i + 1], dest))parse_error("输入了不合法的IP地址");
            free(dest);
            REMOTE_HOST = argv[i + 1];
            i += 2;
        }
        else if (strcmp(field, "log_mask") == 0)
        {
            if (i + 1 == argc)parse_error("缺少参数值");
            int mask = strtol(argv[i + 1], NULL, 10);
            if (mask < 0 || mask > 15)parse_error("mask必须是0-15的整数");
            LOG_MASK = mask;
            i += 2;
        }
        else if (strcmp(field, "client_port") == 0)
        {
            if (i + 1 == argc)parse_error("缺少参数值");
            int port = strtol(argv[i + 1], NULL, 10);
            if (port < 1024 || port > 65535)parse_error("端口必须是1024-65535的整数");
            CLIENT_PORT = port;
            i += 2;
        }
        else if (strcmp(field, "hosts_path") == 0)
        {
            if (i + 1 == argc)parse_error("缺少参数值");
            HOSTS_PATH = argv[i + 1];
            i += 2;
        }
        else if (strcmp(field, "log_path") == 0)
        {
            if (i + 1 == argc)parse_error("缺少参数值");
            LOG_PATH = argv[i + 1];
            i += 2;
        }
        else parse_error("不合法的参数标志");
    }
}
