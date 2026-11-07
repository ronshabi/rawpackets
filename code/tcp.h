#pragma once

#include "ip.h"

#include <stdint.h>

struct tcp_header {
    uint16_t srcport;
    uint16_t dstport;
    uint32_t seq;
    uint32_t ack;
    uint8_t  doffsetbytes;
    struct flags {
        uint8_t cwr : 1;
        uint8_t ece : 1;
        uint8_t urg : 1;
        uint8_t ack : 1;
        uint8_t psh : 1;
        uint8_t rst : 1;
        uint8_t syn : 1;
        uint8_t fin : 1;
    } flags;
    uint32_t window;
    uint16_t checksum;
    uint16_t urgentptr;
};

enum tcp_state {
    TCP_STATE_CLOSED,
    TCP_STATE_LISTEN,
    TCP_STATE_SYN_SENT,
    TCP_STATE_SYN_RECEIVED,
    TCP_STATE_ESTABLISHED,
    TCP_STATE_FIN_WAIT_1,
    TCP_STATE_FIN_WAIT_2,
    TCP_STATE_CLOSE_WAIT,
    TCP_STATE_CLOSING,
    TCP_STATE_LAST_ACK,
    TCP_STATE_TIME_WAIT
};

struct tcb {
    enum tcp_state state;
    uint64_t snd_una;
    uint64_t snd_nxt;
    uint64_t snd_wnd;
    uint64_t snd_up;
    uint64_t snd_wl1;
    uint64_t snd_wl2;
    uint64_t iss;
    uint64_t rcv_nxt;
    uint64_t rcv_wnd;
    uint64_t rcv_up;
    uint64_t irs;
    uint64_t pkt_len;
    uint8_t  pkt_data[MTU];
    uint32_t iphdrlen;
    struct tcp_header th;
};

int  tcp_parse_header(struct tcp_header *th, uint8_t *buf, uint64_t bufsiz);
void tcp_dumphdr(struct tcp_header *tcp);
