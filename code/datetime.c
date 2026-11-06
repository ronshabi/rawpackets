#include "datetime.h"

#include "die.h"

#define _POSIX_C_SOURCE 199309L

#include <time.h>


uint64_t datetime_now_tcpclock()
{
    struct timespec ts;
    int             rc = clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
    if (rc == -1) {
        die("clock gettime failed");
    }

    return ts.tv_sec;
}
