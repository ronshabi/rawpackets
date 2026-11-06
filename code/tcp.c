#include "tcp.h"

#include "bytes.h"

#include <stdio.h>
#include <string.h>

#define BAIL(...)                             \
    printf("! tcp_parse: " __VA_ARGS__ "\n"); \
    return 1

int tcp_parse(struct tcp *tcp, uint8_t *buf, uint64_t bufsiz)
{
    (void)bufsiz; // FIXME
    memset(tcp, 0, sizeof(struct tcp));
    tcp->srcport = read_u16_bswap(buf, 0);
    tcp->dstport = read_u16_bswap(buf, 2);
    tcp->seq     = read_u32_bswap(buf, 4);
    tcp->ack     = read_u32_bswap(buf, 8);

    const uint8_t dataraw  = buf[12];
    const uint8_t flagsraw = buf[13];
    tcp->doffsetbytes      = (dataraw >> 4) & 0x0F;
    tcp->doffsetbytes *= 4; // it's scaled by sizeof(u32)

    tcp->flags.cwr = ((flagsraw & (1 << 7)) != 0);
    tcp->flags.ece = ((flagsraw & (1 << 6)) != 0);
    tcp->flags.urg = ((flagsraw & (1 << 5)) != 0);
    tcp->flags.ack = ((flagsraw & (1 << 4)) != 0);
    tcp->flags.psh = ((flagsraw & (1 << 3)) != 0);
    tcp->flags.rst = ((flagsraw & (1 << 2)) != 0);
    tcp->flags.syn = ((flagsraw & (1 << 1)) != 0);
    tcp->flags.fin = ((flagsraw & (1 << 0)) != 0);

    tcp->window    = read_u16_bswap(buf, 14);
    tcp->checksum  = read_u16_bswap(buf, 16);
    tcp->urgentptr = read_u16_bswap(buf, 18);

    return 0;
}

void tcp_dump(struct tcp *tcp)
{
    printf("TCP | Source Port: %hu\n", tcp->srcport);
    printf("TCP |   Dest Port: %hu\n", tcp->dstport);
    printf("TCP |         SEQ: %u\n", tcp->seq);
    printf("TCP |         ACK: %u\n", tcp->ack);
    printf("TCP |     DOffset: %u bytes\n", (uint32_t)tcp->doffsetbytes);
    printf("TCP |      Window: %u\n", tcp->window);
    printf("TCP |    Checksum: 0x%hx\n", tcp->checksum);
    printf("TCP |   UrgentPtr: %hu\n", tcp->urgentptr);
    printf("TCP |       Flags: (");
    if (tcp->flags.cwr) {
        printf("CWR ");
    }
    if (tcp->flags.ece) {
        printf("ECE ");
    }
    if (tcp->flags.urg) {
        printf("URG ");
    }
    if (tcp->flags.ack) {
        printf("ACK ");
    }
    if (tcp->flags.psh) {
        printf("PSH ");
    }
    if (tcp->flags.rst) {
        printf("RST ");
    }
    if (tcp->flags.syn) {
        printf("SYN ");
    }
    if (tcp->flags.fin) {
        printf("FIN ");
    }
    printf(")\n");
}
