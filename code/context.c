#include "context.h"

#include "datetime.h"
#include "debug.h"
#include "die.h"
#include "tcp_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>

// -----------------------------------------------------------------------------

void context_init(struct context *ctx, const char *tun_name)
{
    DBG("init context");
    memset(ctx, 0, sizeof(struct context));
    tun_init(&ctx->tun, tun_name);
    tcp_pool_init(&ctx->tcp_pool);
}
void context_run(struct context *ctx)
{
    // FIXME: introduce a status variable to mark if running and shit
    for (ctx->nevents = 0;; ++ctx->nevents) {
        DBG("----------------------------------------------------------------");
        DBG("event %lu", ctx->nevents);
        context_run_one(ctx);
    }
}

void context_run_one(struct context *ctx)
{
    struct tcp_pool_item *item = tcp_pool_next_pending(&ctx->tcp_pool);

    if (item) {
        context_process_one_tcp_item(item);
    }

    tun_run_one(&ctx->tun);

    struct basic_packet_info bpi;
    int                      badpacket =
        packet_parse_shallow(ctx->tun.rdbuf, ctx->tun.nbytesrd, &bpi);

    if (badpacket) {
        return;
    }

    // now we should find a slot for this new tcp connection

    if (bpi.protocol == IPPROTO_TCP) {
        context_handle_incoming_tcp(ctx, &bpi);
    } else {
        DBG("unimplemented protocol");
        return;
    }
}

// FIXME: change the name better
void context_process_one_tcp_item(struct tcp_pool_item *item)
{
    // REFACTOR: move into the tcp file
    struct tcb *tcb = item->tcb;

    switch (tcb->state) {

    case TCP_STATE_CLOSED      : die("closed"); break;
    case TCP_STATE_LISTEN      : die("listen"); break;
    case TCP_STATE_SYN_SENT    : die("syn sent"); break;
    case TCP_STATE_SYN_RECEIVED: die("syn received"); break;
    case TCP_STATE_ESTABLISHED : die("state established"); break;
    case TCP_STATE_FIN_WAIT_1  : die("finwait1"); break;
    case TCP_STATE_FIN_WAIT_2  : die("finwait2"); break;
    case TCP_STATE_CLOSE_WAIT  : die("closewait"); break;
    case TCP_STATE_CLOSING     : die("closing"); break;
    case TCP_STATE_LAST_ACK    : die("last ack"); break;
    case TCP_STATE_TIME_WAIT   : die("timewait"); break;
    }
}

// FIXME: change the name
void context_handle_incoming_tcp(struct context           *ctx,
                                 struct basic_packet_info *bpi)
{
    DBG("0x%08X:%hu => 0x%08X:%hu (%u bytes)", bpi->src_addr, bpi->src_port,
        bpi->dst_addr, bpi->dst_port, bpi->len);

    int                   is_new = 0;
    struct tcp_pool      *pool   = &ctx->tcp_pool;
    struct tcp_pool_item *item;

    // try to match first
    item = tcp_pool_match(pool, bpi->src_addr, bpi->dst_addr, bpi->src_port,
                          bpi->dst_port);

    if (!item) {
        DBG("No match for connection, will try to pool alloc");
        is_new = 1;
    }

    if (is_new) {
        item = tcp_pool_next_free(&ctx->tcp_pool);

        if (!item) {
            DBG("no slots available in tcp pool, ignoring this packet :(");
            return;
        }

        // Assign this connection to the pool.
        item->src_ip   = bpi->src_addr;
        item->dst_ip   = bpi->dst_addr;
        item->src_port = bpi->src_port;
        item->dst_port = bpi->dst_port;

        // Allocate new TCB for this connection
        // Note that we reached this place BECAUSE the tcb of this pool item was
        // NULL, meaning when the connection closed, that connection was
        // responsible for freeing its TCB and zeroing the tcb pointer. Failure
        // to do so is a leak, but it's not our fault here.
        item->tcb = malloc(sizeof(struct tcb));
        item->tcb->state = TCP_STATE_LISTEN;
        DBG("init new TCB on LISTEN");
    }

    DBG("Copy %lu bytes into TCB", ctx->tun.nbytesrd);

    if (item->tcb->pkt_len) {
        // FIXME: maybe implement queue?
        DBG("!!! WARNING: There is already data in the TCB packet !!!");
    }
    item->tcb->pkt_len = ctx->tun.nbytesrd;
    memcpy(item->tcb->pkt_data, ctx->tun.rdbuf, ctx->tun.nbytesrd);
}

void context_teardown(struct context *ctx)
{
    tun_teardown(&ctx->tun);
}
