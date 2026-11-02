#pragma once

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

struct rs_ipv4 {
    struct rs_ipv4_hdr hdr;
	char srcaddr_str[RS_IPV4_ADDR_STRING_BUFSIZE];
	char dstaddr_str[RS_IPV4_ADDR_STRING_BUFSIZE];
};