#pragma once

// TODO (vscode): change opt+shift+[ / ] to back and forward.

#include <stddef.h>
#include <stdint.h>
#include <unistd.h> // ssize_t

#define TUN_IFNAMSIZ 16
#define TUN_RDBUFSIZ (1 << 16)

struct tun {
	int fd;
	int error;
	char name[TUN_IFNAMSIZ];
	uint8_t rdbuf[TUN_RDBUFSIZ];
	ssize_t nbytesrd;
};

void tun_init(struct tun *tun, const char *name);
void tun_read(struct tun *tun);
void tun_dump(struct tun *tun);
void tun_teardown(struct tun *tun);
