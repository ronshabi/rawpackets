#include "ip.h"
#include "die.h"

#include <stdio.h>

void ipv4_to_string(uint32_t addr_be, char *out)
{
	const uint8_t *const a = (uint8_t const *)&addr_be;
	int b = snprintf(out, IPV4_ADDR_STR_BUFSIZ, "%d.%d.%d.%d", a[0], a[1],
			 a[2], a[3]);
	if (b >= IPV4_ADDR_STR_BUFSIZ) {
		die("ip to string too long");
	}
}

char *ipv4_proto_to_string(uint8_t proto)
{
	switch (proto) {
	case IPV4_PROTO_ICMP:
		return "ICMP";
	case IPV4_PROTO_TCP:
		return "TCP";
	case IPV4_PROTO_UDP:
		return "UDP";
	default:
		return "???";
	}
}
