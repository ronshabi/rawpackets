#pragma once

#include <stdint.h>

#include "tun.h"

struct context {
    uint64_t nevents;
    struct tun tun;
};

void context_init(struct context *ctx, const char *tun_name);
void context_run(struct context *ctx);
void context_run_one(struct context *ctx);
void context_teardown(struct context *ctx);