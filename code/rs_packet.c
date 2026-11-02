#include "rs_packet.h"
#include "rs_socket.h"

#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "die.h"

int rs_packet_init(struct rs_packet *pkt, const uint8_t *buf, size_t bufsize)
{
	if (rs_ipv4_init(&pkt->ipv4, buf, bufsize)) {
		printf("PACKET INIT FAILED\n");
		return 1;
	}
}