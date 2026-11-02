#pragma once

#include "rs_socket.h"
#include "rs_packet.h"

struct rs_context {
	struct rs_socket socket;
	struct rs_packet packet;
};