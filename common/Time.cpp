#include "Time.h"

#include <sys/time.h>

using namespace CLPIM;

uint32_t Timer::GetCurrentTimeInSec()
{
    timeval t;
    gettimeofday(&t, NULL);

    return t.tv_sec;
}

uint64_t Timer::GetCurrentTimeInMS()
{
    timeval t;
    gettimeofday(&t, NULL);
    uint64_t s = t.tv_sec;
    s += s*1000000+t.tv_usec;

    return s;
}
