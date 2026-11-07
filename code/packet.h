#pragma once

#include <stdint.h>

struct basic_packet_info {
    int protocol;
    uint32_t len;
    uint32_t proto_hdr_offset;
    uint32_t src_addr;
    uint32_t dst_addr;
    uint16_t src_port;
    uint16_t dst_port;
};

int packet_parse_shallow(uint8_t *data, uint64_t len, struct basic_packet_info *bpi);