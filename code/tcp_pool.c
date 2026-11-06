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
    DBG("Trying to find a free slot");
    for (int i = 0; i < POOL_SIZE; ++i) {

        if (pool->items[i].tcb == NULL) {
            DBG("Slot %d @ %p is free", i, &pool->items[i]);
            return &pool->items[i];
        }
    }

    DBG("No free slot found");
    return NULL;
}

struct tcp_pool_item *tcp_pool_next_pending(struct tcp_pool *pool)
{
    DBG("Trying to find a slot to process");
    for (int i = 0; i < POOL_SIZE; ++i) {

        if (pool->items[i].tcb != NULL) {
            DBG("Chose slot %d @ %p to process", i, &pool->items[i]);
            return &pool->items[i];
        }
    }

    DBG("No slots are awaiting processing");
    return NULL;
}
