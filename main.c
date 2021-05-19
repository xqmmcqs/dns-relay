#include <stdio.h>
#include <uv.h>

#include "dns_client.h"
#include "dns_server.h"
#include "cache.h"
#include "util.h"
#include "query_pool.h"

uv_loop_t * loop;
Rbtree * tree;
Query_Pool * qpool;
char log_mask = 15;

int main()
{
    // TODO: 加注释
    // TODO: log
    // TODO: DoH/DoT
    // TODO: EDNS
    // TODO: 命令行参数
    log_info("启动DNS relay")
    loop = uv_default_loop();
    FILE * keep_file = fopen("keep.txt", "r");
    tree = init_cache(keep_file);
    qpool = qpool_init();
    init_client();
    init_server();
    return uv_run(loop, UV_RUN_DEFAULT);
}
