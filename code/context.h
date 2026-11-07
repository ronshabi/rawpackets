#pragma once

#include <stdint.h>

#include "tun.h"
#include "tcp_pool.h"
#include "packet.h"
struct context {
    uint64_t nevents;
    struct tun tun;
    struct tcp_pool tcp_pool;
};

void context_init(struct context *ctx, const char *tun_name);
void context_run(struct context *ctx);
void context_run_one(struct context *ctx);
void context_process_one_tcp_item(struct context *ctx, struct tcp_pool_item *item);
void context_handle_new_tcp(struct context *ctx, struct basic_packet_info *bpi);
void context_teardown(struct context *ctx);
void context_handle_tcp_listen(struct context *ctx, struct tcp_pool_item *item);