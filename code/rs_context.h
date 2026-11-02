#pragma once

#include "rs_packet.h"
#include "rs_socket.h"
#include <stdbool.h>

struct rs_context {
	bool is_running;
	uint64_t nevents;
	struct rs_socket socket;
	struct rs_packet pkt;
};

void rs_context_init(struct rs_context *ctx);
void rs_context_start(struct rs_context *ctx);
void rs_context_teardown(struct rs_context *ctx);
void rs_context_do_one(struct rs_context *ctx);
