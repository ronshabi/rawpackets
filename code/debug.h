#pragma once

#define DBG(fmt, ...)                                      \
    do {                                                   \
        printf("[  %10s] %16s | ", __FILE__, __FUNCTION__); \
        printf(fmt "\n", ##__VA_ARGS__);                   \
    } while (0)
