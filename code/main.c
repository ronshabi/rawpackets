#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define RS_UDP_PROTOCOL_NUMBER 17
#define RS_BUFSIZE (1 << 16)

//
// Good stuff on bitfields
// https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Bit-Field-Packing.html
//
// RFC 791 Header
// https://www.rfc-editor.org/rfc/rfc791.html#section-3.1
//

#define RS_IPV4_HDR_MIN_LENGTH 20
#define RS_IPV4_HDR_VERSION 4
#define RS_IPV4_HDR_IHL_MULTIPLIER 4
#define RS_IPV4_ADDR_STRING_BUFSIZE 16

struct __attribute__((packed)) rs_ipv4_hdr {
	uint8_t ihl : 4;
	uint8_t version : 4;
	uint8_t tos;
	uint16_t len;
	uint16_t ident;
	uint16_t flags_and_off;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint32_t srcaddr;
	uint32_t dstaddr;
};

struct rs_packet {
	struct rs_ipv4_hdr hdr;
	char ip_src_str[RS_IPV4_ADDR_STRING_BUFSIZE];
	char ip_dst_str[RS_IPV4_ADDR_STRING_BUFSIZE];
	char *err;
};

struct rs_socket {
	int fd;
	uint8_t recv_buf[RS_BUFSIZE];
	size_t recv_len;
};

struct rs_ctx {
	struct rs_socket socket;
	struct rs_packet packet;
};

noreturn void die(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

void rs_socket_hexdump(struct rs_socket *s);
void rs_socket_process_recv(struct rs_socket *s);

noreturn void die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "FATAL: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}

void hexdump(uint8_t *buf, size_t buf_size, size_t column_width)
{
	static const char *hexdigits = "0123456789abcdef";
	printf("--- dump %p (%zu)---\n", buf, buf_size);

	for (size_t i = 0; i < buf_size; ++i) {
		uint8_t byte = buf[i];
		char msn = hexdigits[byte >> 4];
		char lsn = hexdigits[byte & 0xF];

		putchar(msn);
		putchar(lsn);

		if ((i + 1) % column_width == 0) {
			putchar('\n');
		} else {
			putchar(' ');
		}
	}

	if (buf_size % column_width != 0) {
		putchar('\n');
	}
}

void rs_socket_init(struct rs_socket *s)
{
	int rc;
	memset(s, 0, sizeof(struct rs_socket));

	//
	// Create the socket
	//
	// I'm using AF_INET + SOCK_RAW without IPPROTO_RAW
	// Since I want to receive packets as well.
	//
	// https://man7.org/linux/man-pages/man7/raw.7.html
	//

	rc = socket(AF_INET, SOCK_RAW, RS_UDP_PROTOCOL_NUMBER);

	if (rc == -1) {
		die("socket (%s)", strerror(errno));
	}

	s->fd = rc;
	printf("* init socket #%d\n", s->fd);

	//
	// Set socket option: IP_HDRINCL
	//
	// INET RAW sockets will always RECEIVE a IP header,
	// but to SEND our crafted header, we need to set IP_HDRINCL as a
	// socket option.
	//
	// https://man7.org/linux/man-pages/man7/raw.7.html
	// https://man7.org/linux/man-pages/man2/setsockopt.2.html
	// https://man7.org/linux/man-pages/man2/IP_HDRINCL.2const.html
	//

	int yes = 1;
	rc = setsockopt(s->fd, IPPROTO_IP, IP_HDRINCL, &yes, sizeof(yes));

	if (rc != 0) {
		die("setsockopt(IPPROTO_IP, IP_HDRINCL, 1): %s",
		    strerror(errno));
	}

	printf("* set IP_HDRINCL\n");
}

void rs_socket_recv(struct rs_socket *s)
{
	printf("* trying to recv(fd: %d, buf: %p, n: %d)\n", s->fd, s->recv_buf,
	       RS_BUFSIZE);
	s->recv_len = recv(s->fd, s->recv_buf, RS_BUFSIZE, 0);
	printf("* received %zd bytes\n", s->recv_len);

	if (s->recv_len < 0 || s->recv_len > RS_BUFSIZE) {
		die("recv length %zd (bufsize is %d)", s->recv_len, RS_BUFSIZE);
	}

	rs_socket_hexdump(s);
}

void rs_socket_hexdump(struct rs_socket *s)
{
	hexdump(s->recv_buf, s->recv_len, 16);
}

void rs_packet_init(struct rs_packet *pkt, const uint8_t *buf, size_t bufsize)
{
	if (!buf || !bufsize || bufsize > RS_BUFSIZE) {
		die("bad buf passed");
	}

	if (bufsize < RS_IPV4_HDR_MIN_LENGTH) {
		die("packet too short for a header (%zu)", bufsize);
	}

	memset(pkt, 0, sizeof(struct rs_packet));
	struct rs_ipv4_hdr *phdr = &pkt->hdr;

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
	printf(" - TOS: %u\n", phdr->tos);
	printf(" - len: %hu\n", phdr->len);
	printf(" - ident: %hu\n", phdr->ident);

	if (inet_ntop(AF_INET, (const void*)&phdr->srcaddr, pkt->ip_src_str, RS_IPV4_ADDR_STRING_BUFSIZE) == NULL) {
		die("inet ntop for srcaddr");
	}

	if (inet_ntop(AF_INET, (const void*)&phdr->dstaddr, pkt->ip_dst_str, RS_IPV4_ADDR_STRING_BUFSIZE) == NULL) {
		die("inet ntop for dstaddr");
	}

	printf(" - src addr: %s\n", pkt->ip_src_str);
	printf(" - dst addr: %s\n", pkt->ip_dst_str);

}

int main()
{
	struct rs_ctx ctx;
	rs_socket_init(&ctx.socket);
	rs_socket_recv(&ctx.socket);
	rs_packet_init(&ctx.packet, ctx.socket.recv_buf, ctx.socket.recv_len);

	return 0;
}