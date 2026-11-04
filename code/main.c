
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
		// tun_dump(&tun);
		struct packet pkt;
		int rc = packet_init(&pkt, tun.rdbuf, tun.nbytesrd);
		if (rc == 0) {
			packet_dump(&pkt, "> ");
		}
	}

	tun_teardown(&tun);

	return 0;
}