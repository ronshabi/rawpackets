#include "rs_context.h"

#include <stdio.h>
#include <string.h>

void rs_context_init(struct rs_context *ctx)
{
        memset(ctx, 0, sizeof(struct rs_context));
	printf("* init context\n");
	rs_socket_init(&ctx->socket);
}

void rs_context_start(struct rs_context *ctx)
{
	ctx->is_running = true;
	while (ctx->is_running) {
                printf("[event %lu]\n", ctx->nevents);
		rs_context_do_one(ctx);
	}
}

void rs_context_do_one(struct rs_context *ctx)
{
        ctx->nevents++;
	rs_socket_recv(&ctx->socket);
	rs_packet_init(&ctx->pkt, ctx->socket.recvbuf, ctx->socket.recvlen);

	// now that we got this packet's general direction, we must attach it to
	// one of the packets in the context.
	// I don't wanna overengineer shit rn so let's just make it an array
	// and search it linearly.
	printf("IMPLEMENT: assign packet %d...\n", ctx->pkt.ipv4.hdr.ident);
}

void rs_context_teardown(struct rs_context *ctx)
{
	rs_socket_teardown(&ctx->socket);
}