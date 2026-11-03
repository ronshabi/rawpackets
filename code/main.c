
// TODO: make clangd/clang-format sort these headers goddamn
#include "tun.h"
#include "packet.h"

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
		packet_dump(&pkt);
	}

	tun_teardown(&tun);

	return 0;
}