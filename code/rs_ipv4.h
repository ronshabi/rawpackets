#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

struct rs_ipv4 {
	uint16_t ihl;
	uint16_t len;
	uint16_t offset;
	uint16_t ident;
	uint32_t srcaddr;
	uint32_t dstaddr;
	uint8_t ttl;
	uint8_t proto;
	bool mf;
	bool df;

	char srcaddr_str[RS_IPV4_ADDR_STRING_BUFSIZE];
	char dstaddr_str[RS_IPV4_ADDR_STRING_BUFSIZE];
};

int rs_ipv4_init(struct rs_ipv4 *st, const uint8_t *buf, size_t bufsize);