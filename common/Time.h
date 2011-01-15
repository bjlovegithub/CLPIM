/**
 * Time Utility
 * by billjeff, Dec/1/2010
 */

#ifndef CLPIM_COMMON_TIME_H
#define CLPIM_COMMON_TIME_H

#include "CommonInclude.h"

namespace CLPIM
{
    
class Timer
{
public:
    static uint32_t GetCurrentTimeInSec();
    static uint64_t GetCurrentTimeInMS();
};

} // namespace CLPIM

#endif // CLPIM_COMMON_TIME_H
