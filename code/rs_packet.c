#include "rs_packet.h"
#include "rs_socket.h"

#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "die.h"

void rs_packet_init(struct rs_packet *pkt, const uint8_t *buf, size_t bufsize)
{
	if (!buf || !bufsize || bufsize > RS_SOCKET_BUFSIZE) {
		die("bad buf passed");
	}

	if (bufsize < RS_IPV4_HDR_MIN_LENGTH) {
		die("packet too short for a header (%zu)", bufsize);
	}

	memset(pkt, 0, sizeof(struct rs_packet));
	struct rs_ipv4_hdr *phdr = &pkt->ipv4.hdr;
	char *p_srcaddr_str = pkt->ipv4.srcaddr_str;
	char *p_dstaddr_str = pkt->ipv4.dstaddr_str;

	// copy at least the basic header which must be there, 20 bytes
	memcpy(phdr, buf, RS_IPV4_HDR_MIN_LENGTH);

	// endianness
	phdr->len = ntohs(phdr->len);
	phdr->ident = ntohs(phdr->ident);
	phdr->flags_and_off = ntohs(phdr->flags_and_off);
	phdr->checksum = ntohs(phdr->checksum);

	printf("Header:\n");
	printf(" - Version: %u\n", phdr->version);
	printf(" - IHL: %d bytes\n",
	       RS_IPV4_HDR_IHL_MULTIPLIER * (int)phdr->ihl);

	if (phdr->ihl * RS_IPV4_HDR_IHL_MULTIPLIER != RS_IPV4_HDR_MIN_LENGTH) {
		// FIXME: implement
		die("this one has header length of %d, idk how to deal with that",
		    phdr->ihl * 4);
	}

	printf(" - TOS: %u\n", phdr->tos);
	printf(" - len: %hu\n", phdr->len);
	printf(" - ident: %hu\n", phdr->ident);

	// REFACTOR: extract function
	if (inet_ntop(AF_INET, (const void *)&phdr->srcaddr, p_srcaddr_str,
		      RS_IPV4_ADDR_STRING_BUFSIZE) == NULL) {
		die("inet ntop for srcaddr");
	}

	if (inet_ntop(AF_INET, (const void *)&phdr->dstaddr, p_dstaddr_str,
		      RS_IPV4_ADDR_STRING_BUFSIZE) == NULL) {
		die("inet ntop for dstaddr");
	}

	printf(" - src addr: %s\n", pkt->ipv4.srcaddr_str);
	printf(" - dst addr: %s\n", pkt->ipv4.dstaddr_str);
}