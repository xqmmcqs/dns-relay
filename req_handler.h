//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_REQ_HANDLER_H
#define DNSR_REQ_HANDLER_H

static uv_udp_t recv_socket;
extern uv_loop_t *loop;

void init_handler();

#endif //DNSR_REQ_HANDLER_H
