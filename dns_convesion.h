//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_CONVESION_H
#define DNSR_DNS_CONVESION_H

#include "dns_structure.h"

void string_to_dnsmsg(Dns_Msg * pmsg, const char *pstring);

void print_dns_string(const char* pstring, int len);

void print_dns_message(const Dns_Msg * pmsg);

#endif //DNSR_DNS_CONVESION_H
