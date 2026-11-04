
// TODO: make clangd/clang-format sort these headers goddamn
#include "tun.h"

#include <stdio.h>

#define QUICKBUFSIZE 256

int main()
{
    struct tun tun;
    tun_init(&tun, "tun69");
    tun_run(&tun);
    tun_teardown(&tun);

    return 0;
}
