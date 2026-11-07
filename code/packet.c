#include "packet.h"

#include "bytes.h"
#include "debug.h"

#include <stdio.h>

#include <netinet/in.h>

int packet_parse_shallow(uint8_t                  *data,
                         uint64_t                  len,
                         struct basic_packet_info *bpi)
{
    if (len < 20) {
        DBG("reject: too small");
        return 1;
    }

    uint8_t version = data[0] >> 4;

    if (version != 4) {
        DBG("reject: not ipv4");
        return 1;
    }

    uint16_t ip_hdr_len_bytes = (data[0] & 0xF) * 4;
    uint16_t ipstatedlen = read_u16_bswap(data, 2);

    // FIXME: implement reassembly.
    if (len != ipstatedlen) {
        DBG("reject: buffer size doesn't match ip header packet size, "
            "reassembly not "
            "implemented yet");
        return 1;
    }

    const uint8_t flags = data[6] >> 13;
    int           df    = (flags >> 1 & 1);
    int           mf    = (flags & 1);

    if (df) {
        DBG("reject: DF not supported");
        return 1;
    }

    if (mf) {
        DBG("reject: MF not supported");
        return 1;
    }

    uint8_t  off1 = data[6] & 0b11111;
    uint8_t  off2 = data[7];
    unsigned off  = (off1 | off2 << 8) * 8; // it's in 8 octet chunks

    // FIXME: implement reassembly.
    if (off) {
        DBG("reject: packets with offsets not supported");
        return 1;
    }

    unsigned ttl = data[8];

    if (ttl == 0) {
        DBG("reject: ttl = 0");
        return 1;
    }

    int proto = data[9];

    if (proto != IPPROTO_TCP) {
        DBG("reject: not TCP");
        return 1;
    }
    
    bpi->protocol = proto;
    bpi->src_addr = read_u32_bswap(data, 12);
    bpi->dst_addr = read_u32_bswap(data, 16);

    // FIXME: magic number
    if (ip_hdr_len_bytes < 20) {
        DBG("reject: tcp header must be atleast 20 bytes");
        return 1;
    }

    bpi->src_port = read_u16_bswap(data, ip_hdr_len_bytes);
    bpi->dst_port = read_u16_bswap(data, ip_hdr_len_bytes + 2);
    bpi->len = len;
    bpi->proto_hdr_offset = ip_hdr_len_bytes;
    
    return 0;
}
