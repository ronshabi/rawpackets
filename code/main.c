
// TODO: make clangd/clang-format sort these headers goddamn
#include <stdio.h>

#include "packet.h"
#include "tun.h"
#include "utils.h"

#define QUICKBUFSIZE 256

int main()
{
	struct tun tun;
	tun_init(&tun, "tun69");

	while (1) {
		tun_read(&tun);
		tun_dump(&tun);
		struct packet pkt;
		packet_init(&pkt, tun.rdbuf, tun.nbytesrd);
		packet_dump(&pkt, "  * ");

		// dump payload
		uint64_t payload_off = pkt.ip.ihlbytes;
		printf("  * payload (offset: %zu)\n", payload_off);
		hexdump(tun.rdbuf + payload_off, pkt.ip.lenbytes - payload_off,
			16, "    | ");
	}

	tun_teardown(&tun);

	return 0;
}