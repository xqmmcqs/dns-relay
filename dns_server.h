//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_SERVER_H
#define DNSR_DNS_SERVER_H

#include <uv.h>

extern uv_loop_t *loop;

void send_to_local(const struct sockaddr *addr, const uv_buf_t * buf);

void init_server();

#endif //DNSR_DNS_SERVER_H
