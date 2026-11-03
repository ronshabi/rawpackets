#include "rs_ipv4.h"

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>


static uint32_t read32u_be(const uint8_t *buf, size_t offset)
{
	uint32_t out;
	memcpy(&out, buf + offset, 4);
	return out;
}

static uint16_t read16u_le(const uint8_t *buf, size_t offset)
{
	return buf[offset] << 8 | buf[offset + 1];
}


static int ip2str(uint32_t addr, char *out)
{
	if (inet_ntop(AF_INET, (const void *)&addr, out,
		      RS_IPV4_ADDR_STRING_BUFSIZE) == NULL) {
		return 1;
	}

	return 0;
}

const char *rs_ipv4_proto_to_string(int proto)
{
	switch (proto) {
		case 6:
			return "TCP";
		case 17:
			return "UDP";
		default:
			return "???";
	}
}

int rs_ipv4_init(struct rs_ipv4 *st, const uint8_t *buf, size_t bufsize)
{
	memset(st, 0, sizeof(struct rs_ipv4));
	int version = buf[0] >> 4;

	// version must be 4
	if (version != VERSION) {
		BAIL("incorrect version");
	}

	unsigned ihl = (unsigned)(buf[0] & 0xF) * 4;

	// ihl too small or too big
	if (ihl < IHLMIN || ihl > IHLMAX) {
		BAIL("ihl not in [20,60]");
	}

        // FIXME: remove this shit when we implement options
        if (ihl != 20) {
		BAIL("header options are not implemented yet");
        }

	st->ihl = ihl;

	// FIXME: should not ignore TOS

	unsigned len = read16u_le(buf, 2);

	// if total length is less than IHL this is a malformed header
	if (len < ihl) {
		BAIL("len < ihl");
	}

	st->len = len;

	st->ident = read16u_le(buf, 4);

	const uint8_t flags = buf[6] >> 13;
	st->df = (bool)(flags >> 1 & 1);
	st->mf = (bool)(flags & 1);

	uint8_t off1 = buf[6] & 0b11111;
	uint8_t off2 = buf[7];
	uint16_t off = (off1 | off2 << 8) * 8; // it's in 8 octet chunks

	// offset can be smaller then len
	// TODO: verify
	if (off > len) {
		BAIL("offset is greater then len");
	}

	st->offset = off;

	st->ttl = buf[8];
	st->proto = buf[9];
	st->srcaddr = read32u_be(buf, 12);
	st->dstaddr = read32u_be(buf, 16);

	if (ip2str(st->srcaddr, st->srcaddr_str)) {
		BAIL("cant convert src addr to str");
	}

	if (ip2str(st->dstaddr, st->dstaddr_str)) {
		BAIL("cant convert dst addr to str");
	}

        return 0;
}