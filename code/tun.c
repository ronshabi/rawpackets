#include "tun.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "die.h"
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

// TODO!: enable asan in alpine and compile with it.

void tun_init(struct tun_context *ctx, const char *name)
{
	memset(ctx, 0, sizeof(struct tun_context));
	if (!name || strlen(name) >= TUN_IFNAMSIZ) {
		die("name is not valid for a tun");
	}

	// clone a tun
	struct ifreq ifr;
	int fd = open("/dev/net/tun", O_RDWR);
	if (fd < 0) {
		// FIXME: use in-struct error
		die("open");
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

	// copy name to tun config
	strncpy(ifr.ifr_name, name, IFNAMSIZ);

	// register
	int rc = ioctl(fd, TUNSETIFF, (void *)&ifr);
	if (rc < 0) {
		die("ioctl tunsetiff");
	}

	// get name of tun into struct context
	strncpy(ctx->tunname, ifr.ifr_name, IFNAMSIZ);
	ctx->tunname[IFNAMSIZ - 1] = '\0';

	// get tun fd into struct context
	ctx->tunfd = fd;

	printf("* setup ok: fd = %d, name = '%s'\n", ctx->tunfd, ctx->tunname);
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
