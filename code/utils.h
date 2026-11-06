#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

void hexdump(uint8_t    *buf,
             size_t      buf_size,
             size_t      column_width,
             const char *before);