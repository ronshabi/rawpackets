#pragma once

// TODO (vscode): change opt+shift+[ / ] to back and forward.

#include <stddef.h>
#include <unistd.h> // ssize_t

#define TUN_IFNAMSIZ 16
#define TUN_RDBUFSIZ (1 << 16)

struct tun_context {
	int tunfd;
    int error;
	char tunname[TUN_IFNAMSIZ];
	char rdbuf[TUN_RDBUFSIZ];
	ssize_t nbytesrd;
};

void tun_init(struct tun_context *ctx, const char *name);
void tun_read(struct tun_context *ctx);
