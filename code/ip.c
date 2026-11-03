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
	case 1:
		return "ICMP";
	case 6:
		return "TCP";
	case 17:
		return "UDP";
	default:
		return "???";
	}
}
