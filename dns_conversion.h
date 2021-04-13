//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_CONVERSION_H
#define DNSR_DNS_CONVERSION_H

#include "dns_structure.h"

void string_to_dnsmsg(Dns_Msg * pmsg, const char * pstring);

unsigned int dnsmsg_to_string(const Dns_Msg * pmsg, char * pstring);

void destroy_dnsrr(Dns_RR * prr);

void destroy_dnsmsg(Dns_Msg * pmsg);

Dns_RR * copy_dnsrr(Dns_RR * src);

Dns_Msg * copy_dnsmsg(Dns_Msg * src);

#endif //DNSR_DNS_CONVERSION_H
