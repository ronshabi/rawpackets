#include "rs_context.h"

#include <stdio.h>
#include <string.h>

int rs_context_init(struct rs_context *ctx)
{
	memset(ctx, 0, sizeof(struct rs_context));
	printf("* init context\n");
	if (rs_socket_init(&ctx->socket)) {
		return 1;
	}
}

int rs_context_start(struct rs_context *ctx)
{
	ctx->is_running = true;
	while (ctx->is_running) {
		ctx->nevents++;
		printf("[event %lu]\n", ctx->nevents);
		if (rs_context_do_one(ctx)) {
			printf("!!! Error in event %lu\n", ctx->nevents);
		}

		printf("%15s > %15s | %5s | ID: %8hu | %hu/%hu",
		       ctx->pkt.ipv4.srcaddr_str, ctx->pkt.ipv4.dstaddr_str,
		       rs_ipv4_proto_to_string(ctx->pkt.ipv4.proto),
		       ctx->pkt.ipv4.ident, ctx->pkt.ipv4.offset,
		       ctx->pkt.ipv4.len);

		if (!ctx->pkt.ipv4.mf) {
			printf(" [L]");
		}

		printf("\n");
	}
}

int rs_context_do_one(struct rs_context *ctx)
{
	if (rs_socket_recv(&ctx->socket)) {
		return 1;
	}

	if (rs_packet_init(&ctx->pkt, ctx->socket.recvbuf,
			   ctx->socket.recvlen)) {
		printf("E: MALFORMED PACKET\n");
		return 1;
	}

	// now that we got this packet's general direction, we must attach it to
	// one of the packets in the context.
	// I don't wanna overengineer shit rn so let's just make it an array
	// and search it linearly.
	printf("IMPLEMENT: assign packet %d...\n", ctx->pkt.ipv4.ident);
	return 0;
}

void rs_context_teardown(struct rs_context *ctx)
{
	rs_socket_teardown(&ctx->socket);
}