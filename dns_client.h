//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_CLIENT_H
#define DNSR_DNS_CLIENT_H

#include "dns_structure.h"
#include <uv.h>

extern uv_loop_t * loop;

void init_client();

void send_to_remote(Dns_Msg * msg);

#endif //DNSR_DNS_CLIENT_H
