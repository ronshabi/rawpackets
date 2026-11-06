#include "context.h"
#include "debug.h"
#include "die.h"

#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    for (ctx->nevents = 0 ;; ++ctx->nevents) {
        DBG("Event %lu", ctx->nevents);
        context_run_one(ctx);
    }
}

void context_run_one(struct context *ctx)
{
    context_process_one_tcp(ctx);

    tun_run_one(&ctx->tun);

    struct basic_packet_info bpi;
    int badpacket = packet_parse_shallow(ctx->tun.rdbuf, ctx->tun.nbytesrd, &bpi);

    if (badpacket) {
        return;
    }

    // now we should find a slot for this new tcp connection

    if (bpi.protocol == IPPROTO_TCP) {
        context_handle_new_tcp(ctx, &bpi);
    } else {
        DBG("unimplemented protocol");
        return;
    }
}
void context_process_one_tcp(struct context *ctx)
{
    DBG("Process one off the TCP pool");
    struct tcp_pool_item *item = tcp_pool_next_pending(&ctx->tcp_pool);
    if (!item) {
        DBG("Nothing to process");
        return;
    }

    DBG("Queue has one connection, umimplemented rn");
    die("unimp");
}

void context_handle_new_tcp(struct context *ctx, struct basic_packet_info *bpi)  {
    DBG("TCP: 0x%08X (%hu) -> 0x%08X (%hu): %u bytes", bpi->src_addr, bpi->src_port, bpi->dst_addr, bpi->dst_port, bpi->len);

    struct tcp_pool_item *item = tcp_pool_next_free(&ctx->tcp_pool);

    if (!item) {
        DBG("no slots available in tcp pool, ignoring this packet :(");
        return;
    }

    // Assign this connection to the pool.
    item->src_ip = bpi->src_addr;
    item->dst_ip = bpi->dst_addr;
    item->src_port = bpi->src_port;
    item->dst_port = bpi->dst_port;

    // Allocate new TCB for this connection
    // Note that we reached this place BECAUSE the tcb of this pool item was
    // NULL, meaning when the connection closed, that connection was responsible
    // for freeing its TCB and zeroing the tcb pointer.
    // Failure to do so is a leak, but it's not our fault here.
    item->tcb = malloc(sizeof (struct tcb));
    DBG("init new TCB");
    die("unimp");
}

void context_teardown(struct context *ctx)
{
    tun_teardown(&ctx->tun);
}
