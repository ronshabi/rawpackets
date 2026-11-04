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

	if (ip->proto != IPV4_PROTO_TCP) {
		BAIL("currently, only TCP is supported");
	}

	uint64_t tcp_start_off = ip->ihlbytes;
	uint64_t rest_of_header = ip->lenbytes - tcp_start_off;
	uint8_t *tcp_start_ptr = buf + tcp_start_off;

	if (tcp_parse(tcp, tcp_start_ptr, rest_of_header)) {
		BAIL("error parsing tcp");
	}

	return 0;
}

void packet_dump(struct packet *pkt)
{
	struct ip *ip = &pkt->ip;
	struct tcp *tcp = &pkt->tcp;
	const char *protostr = ipv4_proto_to_string(ip->proto);

	printf(" IP |   HdrLength: %hu bytes\n", ip->ihlbytes);
	printf(" IP |         TTL: %u\n", (uint32_t)ip->ttl);
	printf(" IP | Source Addr: %s\n", ip->srcaddr_str);
	printf(" IP |   Dest Addr: %s\n", ip->dstaddr_str);
	printf(" IP |    Protocol: %u (%s)\n", (uint32_t)ip->proto, protostr);
	printf(" IP |  Identifier: %hu\n", ip->ident);
	printf(" IP |      Offset: %hu\n", ip->offbytes);
	printf(" IP |      Length: %hu\n", ip->lenbytes);
	printf(" IP |    Checksum: 0x%hx\n", ip->checksum);
	printf("TCP | Source Port: %hu\n", tcp->srcport);
	printf("TCP |   Dest Port: %hu\n", tcp->dstport);
	printf("TCP |         SEQ: %u\n", tcp->seq);
	printf("TCP |         ACK: %u\n", tcp->ack);
	printf("TCP |     DOffset: %u bytes\n", (uint32_t)tcp->doffsetbytes);
	printf("TCP |      Window: %u\n", tcp->window);
	printf("TCP |    Checksum: 0x%hx\n", tcp->checksum);
	printf("TCP |   UrgentPtr: %hu\n", tcp->urgentptr);
	printf("TCP |       Flags: (");
	if (tcp->flags.cwr) {
		printf("CWR ");
	}
	if (tcp->flags.ece) {
		printf("ECE ");
	}
	if (tcp->flags.urg) {
		printf("URG ");
	}
	if (tcp->flags.ack) {
		printf("ACK ");
	}
	if (tcp->flags.psh) {
		printf("PSH ");
	}
	if (tcp->flags.rst) {
		printf("RST ");
	}
	if (tcp->flags.syn) {
		printf("SYN ");
	}
	if (tcp->flags.fin) {
		printf("FIN ");
	}
	printf(")\n");
}