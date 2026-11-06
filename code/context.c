#include "context.h"
#include "debug.h"

#include <string.h>
#include <stdio.h>


// -----------------------------------------------------------------------------

void context_init(struct context *ctx, const char *tun_name)
{
    DBG("init context");
    memset(ctx, 0, sizeof(struct context));
    tun_init(&ctx->tun, tun_name);
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
    tun_run_one(&ctx->tun);
    
    struct basic_packet_info bpi;
    int badpacket = packet_parse_shallow(ctx->tun.rdbuf, ctx->tun.nbytesrd, &bpi);

    if (!badpacket) {
        DBG("0x%08X (%hu) -> 0x%08X (%hu): %u bytes", bpi.src_addr, bpi.src_port, bpi.dst_addr, bpi.dst_port, bpi.len);
    }
}

void context_teardown(struct context *ctx)
{
    tun_teardown(&ctx->tun);
}
