#include "ip.h"

#include "die.h"
#include "utils.h"

#include <stdio.h>

#define VERSION 4
#define IHLMIN  20
#define IHLMAX  60

void ipv4_to_string(uint32_t addr_be, char *out)
{
    const uint8_t *const a = (uint8_t const *)&addr_be;
    int b = snprintf(out, IPV4_ADDR_STR_BUFSIZ, "%d.%d.%d.%d", a[0], a[1], a[2],
                     a[3]);
    if (b >= IPV4_ADDR_STR_BUFSIZ) {
        die("ip to string too long");
    }
}

char *ipv4_proto_to_string(uint8_t proto)
{
    switch (proto) {
    case IPV4_PROTO_ICMP: return "ICMP";
    case IPV4_PROTO_TCP : return "TCP";
    case IPV4_PROTO_UDP : return "UDP";
    default             : return "???";
    }
}

#define BAIL(...)                            \
    printf("! ip_parse: " __VA_ARGS__ "\n"); \
    return 1

int ip_parse(struct ip *ip, uint8_t *buf, uint64_t bufsiz)
{
    memset(ip, 0, sizeof(struct ip));
    ip->version = buf[0] >> 4;

    if (bufsiz < IHLMIN) {
        BAIL("buffer too small");
    }

    if (ip->version != VERSION) {
        BAIL("version not 4");
    }

    ip->ihlbytes = (unsigned)(buf[0] & 0xF) * 4;
    // ihl too small or too big
    if (ip->ihlbytes < IHLMIN || ip->ihlbytes > IHLMAX) {
        BAIL("ihl not in [20,60]");
    }

    // FIXME: should not ignore TOS

    ip->lenbytes = read_u16_bswap(buf, 2);

    // FIXME: implement reassembly.
    if (bufsiz != ip->lenbytes) {
        BAIL("buffer size doesn't match ip header packet size, reassembly not "
             "implemented yet");
    }

    // if total length is less than IHL this is a malformed header
    if (ip->lenbytes < ip->ihlbytes) {
        BAIL("len < ihl");
    }

    ip->ident = read_u16_bswap(buf, 4);

    const uint8_t flags = buf[6] >> 13;
    ip->dontfragment    = (flags >> 1 & 1);
    ip->morefragments   = (flags & 1);

    // FIXME: implement reassembly.
    if (ip->morefragments) {
        BAIL("MF=1, reassembly is not implemented yet");
    }

    uint8_t off1 = buf[6] & 0b11111;
    uint8_t off2 = buf[7];
    ip->offbytes = (off1 | off2 << 8) * 8; // it's in 8 octet chunks

    // FIXME: implement reassembly.
    if (ip->offbytes) {
        BAIL("this packet has an offset, reassembly is not implemented yet");
    }

    if (ip->offbytes > ip->lenbytes) {
        BAIL("offset is greater then len");
    }

    ip->ttl     = buf[8];
    ip->proto   = buf[9];
    ip->srcaddr = read_u32(buf, 12);
    ip->dstaddr = read_u32(buf, 16);

    ipv4_to_string(ip->srcaddr, ip->srcaddr_str);
    ipv4_to_string(ip->dstaddr, ip->dstaddr_str);
    return 0;
}
void ip_dump(struct ip *ip)
{
    const char *protostr = ipv4_proto_to_string(ip->proto);

    printf(" IP |   HdrLength: %hu bytes\n", ip->ihlbytes);
    printf(" IP |         TTL: %u\n", (uint32_t)ip->ttl);
    printf(" IP | Source Addr: %s\n", ip->srcaddr_str);
    printf(" IP |   Dest Addr: %s\n", ip->dstaddr_str);
    printf(" IP |    Protocol: %u (%s)\n", (uint32_t)ip->proto, protostr);
    printf(" IP |  Identifier: %hu\n", ip->ident);
    printf(" IP |      Offset: %hu\n", ip->offbytes);
    printf(" IP |      Length: %hu\n", ip->lenbytes);
    printf(" IP |    Checksum: 0x%hx\n", ip->checksum);
}
