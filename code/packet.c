#include "packet.h"

#include <stdio.h>

#define BAIL(...)                                   \
	printf("! packet_init: " __VA_ARGS__ "\n"); \
	return 1

// *****************************************************************************

int packet_init(struct packet *pkt, uint8_t *buf, uint64_t bufsiz)
{
	struct ip *ip = &pkt->ip;
	struct tcp *tcp = &pkt->tcp;

	// FIXME: parse* functions should take a const pointer to buf.
	if (ip_parse(ip, buf, bufsiz)) {
		BAIL("error parsing ip header");
	}

	ip_dump(ip);

	if (ip->proto != IPV4_PROTO_TCP) {
		BAIL("currently, only TCP is supported");
	}

	uint64_t tcp_start_off = ip->ihlbytes;
	uint64_t rest_of_header = ip->lenbytes - tcp_start_off;
	uint8_t *tcp_start_ptr = buf + tcp_start_off;

	if (tcp_parse(tcp, tcp_start_ptr, rest_of_header)) {
		BAIL("error parsing tcp");
	}

	tcp_dump(tcp);

	return 0;
}