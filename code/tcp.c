#include "tcp.h"
#include "die.h"
#include "bytes.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

#define BAIL(...)                             \
    printf("! tcp_parse: " __VA_ARGS__ "\n"); \
    return 1

#define BASE_HDR_SZ (5 * 4)

int tcp_parse_header(struct tcp_header *th, uint8_t *buf, uint64_t bufsiz)
{
    (void)bufsiz; // FIXME
    memset(th, 0, sizeof(struct tcp_header));
    th->srcport = read_u16_bswap(buf, 0);
    th->dstport = read_u16_bswap(buf, 2);
    th->seq     = read_u32_bswap(buf, 4);
    th->ack     = read_u32_bswap(buf, 8);

    const uint8_t dataraw  = buf[12];
    const uint8_t flagsraw = buf[13];
    th->doffsetbytes      = (dataraw >> 4) & 0x0F;
    th->doffsetbytes *= 4; // it's scaled by sizeof(u32)

    th->flags.cwr = ((flagsraw & (1 << 7)) != 0);
    th->flags.ece = ((flagsraw & (1 << 6)) != 0);
    th->flags.urg = ((flagsraw & (1 << 5)) != 0);
    th->flags.ack = ((flagsraw & (1 << 4)) != 0);
    th->flags.psh = ((flagsraw & (1 << 3)) != 0);
    th->flags.rst = ((flagsraw & (1 << 2)) != 0);
    th->flags.syn = ((flagsraw & (1 << 1)) != 0);
    th->flags.fin = ((flagsraw & (1 << 0)) != 0);

    th->window    = read_u16_bswap(buf, 14);
    th->checksum  = read_u16_bswap(buf, 16);
    th->urgentptr = read_u16_bswap(buf, 18);

    if (th->doffsetbytes != BASE_HDR_SZ) {
        hexdump(buf, bufsiz, 16, " | ");
        die("tcp packet has options");

    }

    return 0;
}

void tcp_dumphdr(struct tcp_header *tcp)
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
