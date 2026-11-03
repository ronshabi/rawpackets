#pragma once

#include <stdint.h>
#include "ip.h"

struct packet {
    // stuff pertaining to rfc 791
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
    } ip;
    struct tcp {
        uint16_t srcport;
        uint16_t dstport;
        uint32_t seq;
        uint32_t ack;
    } tcp;
};

int packet_init(struct packet *pkt, uint8_t *buf, uint64_t bufsiz);
void packet_dump(struct packet *pkt, const char *prefix);
