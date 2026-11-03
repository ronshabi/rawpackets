#include "packet.h"

#include <stdio.h>
#include <string.h>

#define VERSION 4
#define IHLMIN 20
#define IHLMAX 60

#define BAIL(...)                                   \
	printf("! packet_init: " __VA_ARGS__ "\n"); \
	return 1

// *****************************************************************************

// Does not byteswap!
static uint32_t read_u32(const uint8_t *buf, uint64_t offset)
{
	uint32_t out;
	memcpy(&out, buf + offset, 4);
	return out;
}

// Does byteswap
static uint16_t read_u16_bswap(const uint8_t *buf, uint64_t offset)
{
	return buf[offset] << 8 | buf[offset + 1];
}

static int parseip(struct ip *ip, uint8_t *buf, uint64_t bufsiz)
{
	memset(ip, 0, sizeof(struct ip));
	ip->version = buf[0] >> 4;

        if (bufsiz < IHLMIN) {
                BAIL("buffer too small");
        }

	if (ip->version != VERSION) {
		BAIL("version not 4");
	}

	ip->ihlbytes = (unsigned)(buf[0] & 0xF) * 4;
	// ihl too small or too big
	if (ip->ihlbytes < IHLMIN || ip->ihlbytes > IHLMAX) {
		BAIL("ihl not in [20,60]");
	}

	// FIXME: should not ignore TOS

	ip->lenbytes = read_u16_bswap(buf, 2);

        // FIXME: implement reassembly.
        if (bufsiz != ip->lenbytes) {
                BAIL("buffer size doesn't match ip header packet size, reassembly not implemented yet");
        }

	// if total length is less than IHL this is a malformed header
	if (ip->lenbytes < ip->ihlbytes) {
		BAIL("len < ihl");
	}

	ip->ident = read_u16_bswap(buf, 4);


	const uint8_t flags = buf[6] >> 13;
	ip->dontfragment = (flags >> 1 & 1);
	ip->morefragments = (flags & 1);

        // FIXME: implement reassembly.
        if (ip->morefragments) {
                BAIL("MF=1, reassembly is not implemented yet");
        }

	uint8_t off1 = buf[6] & 0b11111;
	uint8_t off2 = buf[7];
	ip->offbytes = (off1 | off2 << 8) * 8; // it's in 8 octet chunks

        // FIXME: implement reassembly.
        if (ip->offbytes) {
                BAIL("this packet has an offset, reassembly is not implemented yet");
        }


	if (ip->offbytes > ip->lenbytes) {
		BAIL("offset is greater then len");
	}



	ip->ttl = buf[8];
	ip->proto = buf[9];
	ip->srcaddr = read_u32(buf, 12);
	ip->dstaddr = read_u32(buf, 16);

	ipv4_to_string(ip->srcaddr, ip->srcaddr_str);
	ipv4_to_string(ip->dstaddr, ip->dstaddr_str);
	return 0;
}

// *****************************************************************************

int packet_init(struct packet *pkt, uint8_t *buf, uint64_t bufsiz)
{
        struct ip *ip = &pkt->ip;
        if (parseip(ip, buf, bufsiz)) {
                BAIL("error parsing ip header");
        }
        return 0;
}

void packet_dump(struct packet *pkt, const char *prefix)
{
	struct ip *ip = &pkt->ip;
	const char *protostr = ipv4_proto_to_string(ip->proto);

	printf("%s%s > %s | %5s | ID: %8hu | %hu/%hu\n", prefix,
	       ip->srcaddr_str, ip->dstaddr_str, protostr, ip->ident,
	       ip->offbytes, ip->lenbytes);
}