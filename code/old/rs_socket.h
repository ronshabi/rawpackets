#pragma once

#include <stdint.h>
#include <stddef.h>

#define RS_SOCKET_BUFSIZE (1 << 16)
#define RS_TCP_PROTOCOL_NUMBER 6
#define RS_UDP_PROTOCOL_NUMBER 17

struct rs_socket {
	int fd;
	uint8_t recvbuf[RS_SOCKET_BUFSIZE];
	size_t recvlen;
};

int rs_socket_init(struct rs_socket *s);
int rs_socket_recv(struct rs_socket *s);
void rs_socket_hexdump(struct rs_socket *s);
void rs_socket_teardown(struct rs_socket *s);