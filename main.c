#include <stdlib.h>
#include <uv.h>
#include "dns_server.h"
#include "req_handler.h"

uv_loop_t *loop, *server_loop;

static void init_server()
{
    uv_loop_init(server_loop);
}

int main()
{
    // TODO: 事件循环不起来
    loop = uv_default_loop();
    uv_thread_t init_thread;
    server_loop = malloc(sizeof(uv_loop_t));
    uv_thread_create(&init_thread, init_server, 0);
    init_handler();
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
