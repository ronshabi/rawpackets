#pragma once

#include "ip.h"
#include "tcp.h"

#include <stdint.h>

struct packet {
    struct ip  ip;
    struct tcp tcp;
};

int  packet_init(struct packet *pkt, uint8_t *buf, uint64_t bufsiz);
void packet_dump(struct packet *pkt);
