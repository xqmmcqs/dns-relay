//
// Created by xqmmcqs on 2021/4/6.
//

#ifndef DNSR_DNS_PRINT_H
#define DNSR_DNS_PRINT_H

#include "dns_structure.h"

void print_dns_string(const char * pstring, unsigned int len);

void print_dns_message(const Dns_Msg * pmsg);

#endif //DNSR_DNS_PRINT_H
