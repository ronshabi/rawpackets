#include "context.h"

#include <stdio.h>


int main()
{
    struct context ctx;
    context_init(&ctx, "tun69");
    context_run(&ctx);
    context_teardown(&ctx);
    return 0;
}
