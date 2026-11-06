#pragma once

// TODO (vscode): change opt+shift+[ / ] to back and forward.

#include "packet.h"

#include <stddef.h>
#include <stdint.h>

#include <unistd.h> // ssize_t

#define TUN_IFNAMSIZ 16
#define TUN_RDBUFSIZ (1 << 16)

struct tun {
    int           fd;
    int           error;
    uint64_t      n_read_events;
    uint64_t      n_write_events;
    uint64_t      n_packet_errors;
    char          name[TUN_IFNAMSIZ];
    uint8_t       rdbuf[TUN_RDBUFSIZ];
    ssize_t       nbytesrd;
    struct packet lastpkt;
};

void tun_init(struct tun *tun, const char *name);
void tun_read(struct tun *tun);
int  tun_run_one(struct tun *tun);
void tun_teardown(struct tun *tun);
