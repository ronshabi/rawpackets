#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

void hexdump(uint8_t    *buf,
             size_t      buf_size,
             size_t      column_width,
             const char *before);

// Does not byteswap!
static inline uint32_t read_u32(const uint8_t *buf, uint64_t offset)
{
    uint32_t out;
    memcpy(&out, buf + offset, 4);
    return out;
}

// Does byteswap
static inline uint16_t read_u16_bswap(const uint8_t *buf, uint64_t offset)
{
    return buf[offset] << 8 | buf[offset + 1];
}

static inline uint32_t read_u32_bswap(const uint8_t *buf, uint64_t offset)
{
    const uint32_t x = *(uint32_t *)(buf + offset);
    return (x & 0x000000FF) << 24 | (x & 0x0000FF00) << 8 |
           (x & 0x00FF0000) >> 8 | (x & 0xFF000000) >> 24;
}
