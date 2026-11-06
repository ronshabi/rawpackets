#pragma once

#include <stdint.h>
#include <stddef.h>

#define POOL_SIZE 4

struct tcb {

};

struct tcp_pool_item {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    struct tcb* tcb;
};

struct tcp_pool {
    struct tcp_pool_item items[POOL_SIZE];
};

void tcp_pool_init(struct tcp_pool *pool);

/// NULL if no slot is found
struct tcp_pool_item* tcp_pool_next_free(struct tcp_pool *pool);

/// NULL if there is nothing to process
struct tcp_pool_item* tcp_pool_next_pending(struct tcp_pool *pool);
