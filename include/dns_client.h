//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_CLIENT_H
#define DNSR_DNS_CLIENT_H

#include <uv.h>

#include "dns_structure.h"

void init_client(uv_loop_t * loop);

void send_to_remote(const Dns_Msg * msg);

#endif //DNSR_DNS_CLIENT_H
