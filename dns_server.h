//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_SERVER_H
#define DNSR_DNS_SERVER_H

#include <uv.h>

extern uv_loop_t *server_loop;

void run_server();

uv_buf_t *redirect(const uv_buf_t *buf);

#endif //DNSR_DNS_SERVER_H
