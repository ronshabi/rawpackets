#pragma once

#include <stdint.h>

/// Return a valid clock time for the implementation of TCP
/// Might be changed in the future to provide a better clock.
uint64_t datetime_now_tcpclock();