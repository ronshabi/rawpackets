// FIXME: remove these includes, functions should really be in the context.
#include "rs_context.h"
#include "rs_packet.h"
#include "rs_socket.h"

int main()
{
	struct rs_context ctx;
	rs_socket_init(&ctx.socket);
	rs_socket_recv(&ctx.socket);
	rs_packet_init(&ctx.packet, ctx.socket.recv_buf, ctx.socket.recv_len);

	return 0;
}