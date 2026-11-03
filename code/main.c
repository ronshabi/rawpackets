
// TODO: make clangd/clang-format sort these headers goddamn
#include "tun.h"

#define QUICKBUFSIZE 256



int main()
{
	struct tun_context ctx;
	tun_init(&ctx, "tun69");
	tun_read(&ctx);

	return 0;
}