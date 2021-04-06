#include <uv.h>
#include "dns_client.h"
#include "dns_server.h"
#include "util.h"

uv_loop_t *loop;

int main()
{
    print_log(DEBUG, "Starting main");
    loop = uv_default_loop();
    init_client();
    init_server();
    return uv_run(loop, UV_RUN_DEFAULT);
}
