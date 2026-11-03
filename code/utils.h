#pragma once

#include <stdint.h>
#include <stddef.h>

void hexdump(uint8_t *buf, size_t buf_size, size_t column_width, const char *before);
