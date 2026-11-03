#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "die.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <unistd.h>

// https://docs.kernel.org/networking/tuntap.html
// https://man7.org/linux/man-pages/man7/netdevice.7.html

// set vhdr size
// https://blog.cloudflare.com/virtual-networking-101-understanding-tap/
// https://elixir.bootlin.com/linux/v6.4.6/source/include/uapi/linux/virtio_net.h#L187
// https://elixir.bootlin.com/qemu/v8.1.1/source/net/tap-linux.c#L120
// https://bugzilla.redhat.com/show_bug.cgi?id=508861
// https://elixir.bootlin.com/linux/v6.4.6/source/include/linux/if_tap.h#L63
// https://elixir.bootlin.com/linux/v6.4.6/source/drivers/net/tap.c#L1077
// VHDR sizes are either 10,12 or 20. from:
// https://docs.oasis-open.org/virtio/virtio/v1.2/csd01/virtio-v1.2-csd01.html

#define QUICKBUFSIZE 256
#define TUN_RDBUFSIZ (1 << 16)

struct tun_context {
	int tunfd;
	int shouldexist;
	char tunname_hint[IFNAMSIZ];
	char tunname[IFNAMSIZ];
	char rdbuf[TUN_RDBUFSIZ];
	ssize_t nbytesrd;
};

void tun_setnamehint(struct tun_context *ctx, const char *name)
{
	if (strlen(name) >= IFNAMSIZ) {
		die("name '%s' too long", name);
	}

	strncpy(ctx->tunname_hint, name, IFNAMSIZ);
	ctx->shouldexist = 1;
	printf("* set tun name hint to '%s'\n", name);
}
/*

https://docs.cyanlab.org/en/stable/CNIT481-SDN/source/labs/linux_bridges.html

equiv setup in iproute2:

ip tuntap add dev tun69 mod tun
ip addr add 10.0.0.1/24 dev tun69
ip link set dev tun69 up

down
ip link set dev tun69 down

deletion:
ip tuntap del dev tun69 mode tun

*/
int tun_init(struct tun_context *ctx)
{
	struct ifreq ifr;
	int fd = open("/dev/net/tun", O_RDWR);
	if (fd < 0) {
		die("open");
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

	if (ctx->shouldexist) {
		strncpy(ifr.ifr_name, ctx->tunname_hint, IFNAMSIZ);
	}

	// register
	int rc = ioctl(fd, TUNSETIFF, (void *)&ifr);
	if (rc < 0) {
		die("ioctl tunsetiff");
	}

	strncpy(ctx->tunname, ifr.ifr_name, IFNAMSIZ);
	ctx->tunname[IFNAMSIZ - 1] = '\0';
	ctx->tunfd = fd;
	printf("* setup ok: fd = %d, name = '%s'\n", ctx->tunfd, ctx->tunname);
	return 0;
}

void tun_read(struct tun_context *ctx)
{
	printf("* tun_read\n");
	ctx->nbytesrd = read(ctx->tunfd, ctx->rdbuf, TUN_RDBUFSIZ);
	if (ctx->nbytesrd < 0) {
		die("nbytesrd = %zd", ctx->nbytesrd);
	}
	printf("> %zd bytes\n", ctx->nbytesrd);
}

int main()
{
	struct tun_context ctx;
	tun_setnamehint(&ctx, "tun69");
	if (tun_init(&ctx)) {
		die("failed setup()");
	}

	tun_read(&ctx);

	return 0;
}