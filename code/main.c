
// TODO: make clangd/clang-format sort these headers goddamn
#include <stdio.h>

#include "packet.h"
#include "tun.h"
#include "utils.h"

#define QUICKBUFSIZE 256

int main()
{
	struct tun tun;
	tun_init(&tun, "tun69");
	tun_run(&tun);
	tun_teardown(&tun);

	return 0;
}