#pragma once

#include <stdint.h>

#define IPV4_ADDR_STR_BUFSIZ 16
#define IPV4_PROTO_ICMP 1
#define IPV4_PROTO_TCP 6
#define IPV4_PROTO_UDP 17

struct ip {
        uint8_t version;
        uint8_t ihlbytes;
        uint8_t ttl;
        uint8_t proto;
        uint16_t lenbytes;
        uint16_t ident;
        uint16_t offbytes;
        uint16_t checksum;
        uint32_t srcaddr;
        uint32_t dstaddr;
        int dontfragment;
        int morefragments;
        char srcaddr_str[IPV4_ADDR_STR_BUFSIZ];
        char dstaddr_str[IPV4_ADDR_STR_BUFSIZ];
};

// NOTE: We assume the out buffer has enough space to print an ipv4.
// The maximum space is:
//	4 x 3 chars = 12
//      3 x 1 dot   =  3
//      1 x nulterm =  1
//      ----------------
//                    16
//
void ipv4_to_string(uint32_t addr_be, char *out);

char *ipv4_proto_to_string(uint8_t proto);

int ip_parse(struct ip* ip, uint8_t *buf, uint64_t bufsiz);
void ip_dump(struct ip *ip);