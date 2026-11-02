#pragma once

#include "rs_ipv4.h"

#include <stdint.h>
#include <stddef.h>

struct rs_packet {
    struct rs_ipv4 ipv4;
	char *err;
};

void rs_packet_init(struct rs_packet *pkt, const uint8_t *buf, size_t bufsize);