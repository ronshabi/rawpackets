#include "tun.h"

#include "debug.h"
#include "die.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>

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

// TODO!: enable asan in alpine and compile with it.

void tun_init(struct tun *tun, const char *name)
{
    DBG("init tun '%s'", name);
    memset(tun, 0, sizeof(struct tun));
    if (!name || strlen(name) >= TUN_IFNAMSIZ) {
        die("name is not valid for a tun");
    }

    // clone a tun

    DBG("clone");
    struct ifreq ifr;
    int          fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0) {
        // FIXME: use in-struct error
        die("open");
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    // copy name to tun config
    strncpy(ifr.ifr_name, name, IFNAMSIZ);

    // register
    DBG("ioctl");
    int rc = ioctl(fd, TUNSETIFF, (void *)&ifr);
    if (rc < 0) {
        die("ioctl");
    }

    // get name of tun into struct context
    strncpy(tun->name, ifr.ifr_name, IFNAMSIZ);
    tun->name[IFNAMSIZ - 1] = '\0';

    // get tun fd into struct context
    tun->fd = fd;

    DBG("ok: fd=%d, name='%s'", tun->fd, tun->name);
}

void tun_read(struct tun *tun)
{
    tun->nbytesrd = read(tun->fd, tun->rdbuf, TUN_RDBUFSIZ);
    tun->n_read_events++;
    if (tun->nbytesrd < 0) {
        die("nbytesrd = %zd", tun->nbytesrd);
    }
}

int tun_run_one(struct tun *tun)
{
    tun_read(tun);
    printf("%zu\n", tun->n_read_events);

    int rc = packet_init(&tun->lastpkt, tun->rdbuf, tun->nbytesrd);

    if (rc) {
        tun->n_packet_errors++;
        return 1;
    }

    putchar('\n');

    return 0;
}

void tun_run(struct tun *tun)
{
    while (1) {
        tun_run_one(tun); // FIXME: don't ignore return val
    }
}

void tun_teardown(struct tun *tun)
{
    printf("* closing tun #%d\n", tun->fd);
    close(tun->fd);
}
