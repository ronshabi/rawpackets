#pragma once

#include <stdint.h>

struct tcp {
	uint16_t srcport;
	uint16_t dstport;
	uint32_t seq;
	uint32_t ack;
	uint8_t doffsetbytes;
	struct flags {
		uint8_t cwr : 1;
		uint8_t ece : 1;
		uint8_t urg : 1;
		uint8_t ack : 1;
		uint8_t psh : 1;
		uint8_t rst : 1;
		uint8_t syn : 1;
		uint8_t fin : 1;
	} flags;
	uint32_t window;
	uint16_t checksum;
	uint16_t urgentptr;
};

int tcp_parse(struct tcp *tcp, uint8_t *buf, uint64_t bufsiz);
void tcp_dump(struct tcp *tcp);
