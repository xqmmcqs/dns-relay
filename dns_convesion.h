//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_CONVESION_H
#define DNSR_DNS_CONVESION_H

#include "dns_structure.h"

void string_to_dnsmsg(Dns_Msg * pmsg, const char * pstring);

unsigned int dnsmsg_to_string(const Dns_Msg * pmsg, char * pstring);

void destroy_dnsmsg(Dns_Msg * pmsg);

#endif //DNSR_DNS_CONVESION_H
