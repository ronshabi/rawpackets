#include "tcp_pool.h"

#include "debug.h"

#include <stdio.h>
#include <string.h>

void tcp_pool_init(struct tcp_pool *pool)
{
    DBG("init with max %d connections", POOL_SIZE);

    memset(pool->items, 0, sizeof(struct tcp_pool_item) * POOL_SIZE);
}
struct tcp_pool_item *tcp_pool_next_free(struct tcp_pool *pool)
{
    // DBG("Trying to find a free slot");
    for (int i = 0; i < POOL_SIZE; ++i) {

        if (pool->items[i].tcb == NULL) {
            DBG("slot %d @ %p is free", i, &pool->items[i]);
            return &pool->items[i];
        }
    }
    return NULL;
}

struct tcp_pool_item *tcp_pool_next_pending(struct tcp_pool *pool)
{
    for (int i = 0; i < POOL_SIZE; ++i) {

        if (pool->items[i].tcb != NULL) {
            DBG("process #%d (%p)", i, &pool->items[i]);
            return &pool->items[i];
        }
    }
    return NULL;
}

struct tcp_pool_item *tcp_pool_match(struct tcp_pool *pool,
                                     uint32_t         src_ip,
                                     uint32_t         dst_ip,
                                     uint16_t         src_port,
                                     uint16_t         dst_port)
{
    DBG("0x%08X:%hu => 0x%08X:%hu", src_ip, src_port, dst_ip, dst_port);

    for (int i = 0; i < POOL_SIZE; ++i) {

        struct tcp_pool_item *it = &pool->items[i];

        int has_tcb   = it->tcb != NULL;
        int match_src = it->src_ip == src_ip && it->src_port == src_port;
        int match_dst = it->dst_ip == dst_ip && it->dst_port == dst_port;

        if (has_tcb && match_src && match_dst) {
            return it;
        }
    }

    return NULL;
}
