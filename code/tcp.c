#include "tcp.h"

#include "bytes.h"
#include "debug.h"
#include "die.h"
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
    th->doffsetbytes       = (dataraw >> 4) & 0x0F;
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
        th->options_size = (th->doffsetbytes - BASE_HDR_SZ);
        hexdump(buf, BASE_HDR_SZ, 4, "tcphdr > ");
        hexdump(buf + BASE_HDR_SZ, th->options_size, 4, "tcpopt > ");
        if (tcp_parse_options(th, buf, bufsiz)) {
            DBG("reject: bad options");
            return 1;
        }
        die("tcp packet has options of size %u", th->options_size);
    }

    return 0;
}

enum optsmstate {
    OPSM_KIND,
    OPSM_LEN,
    OPSM_VAL
};

#define TCPOPT_EOL        0
#define TCPOPT_NOP        1
#define TCPOPT_MSS        2
#define TCPOPT_MSS_LENGTH 4

int tcp_parse_options(struct tcp_header *th, uint8_t *buf, uint64_t bufsiz)
{
    enum optsmstate st          = OPSM_KIND;
    uint8_t         kind        = 0;
    uint8_t         len         = 0;
    int             had_options = 0;

    uint8_t       *p   = buf + th->options_size;
    uint8_t *const end = buf + bufsiz;

    while (p < end) {
        had_options = 1;
        switch (st) {

        case OPSM_KIND:
            kind = p[0];
            if (kind == TCPOPT_EOL) {
                DBG("got EOL option, skipping all of the rest");
                th->options.eol = 1;
                p               = end;
            } else if (kind == TCPOPT_NOP) {
                DBG("got NOP option");
                th->options.nop = 1;
                st              = OPSM_KIND;
            } else {
                DBG("got option %d", kind);
                st = OPSM_LEN;
            }
            ++p;
            break;
        case OPSM_LEN:
            len = p[0];
            if (kind == TCPOPT_MSS) {
                DBG("MSS length is %d", len);
                if (len != TCPOPT_MSS_LENGTH) {
                    DBG("bad mss length, expected %d, got %d",
                        TCPOPT_MSS_LENGTH, len);
                    return 1;
                }
            } else {
                DBG("length for unknown option is %d", len);
            }
            st = OPSM_VAL;
            ++p;
            break;
        case OPSM_VAL:
            if (kind == TCPOPT_MSS) {
                th->options.mss = read_u16_bswap(p, 0);
                DBG("MSS value: %u", th->options.mss);
                p += 2;
                st = OPSM_KIND;
            } else {
                DBG("skipping unknown option value");
            }

            st = OPSM_KIND;
            p += len - 2; // MINUS 2 CUZ LEN includes the KIND+LEN bytes!
            break;
        }
    }

    if (!had_options) {
        die("we had options but didn't parse them? check your pointers");
    }

    return 0;
}

void tcp_dumphdr(struct tcp_header *tcp)
{
    printf("TCP | Source Port: %hu\n", tcp->srcport);
    printf("TCP |   Dest Port: %hu\n", tcp->dstport);
    printf("TCP |         SEQ: %u\n", tcp->seq);
    printf("TCP |         ACK: %u\n", tcp->ack);
    printf("TCP |     DOffset: %u bytes\n", tcp->doffsetbytes);
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
