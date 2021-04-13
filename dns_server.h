//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_SERVER_H
#define DNSR_DNS_SERVER_H

#include "dns_structure.h"
#include <uv.h>

void init_server();

void send_to_local(const struct sockaddr * addr, Dns_Msg * msg);

#endif //DNSR_DNS_SERVER_H
