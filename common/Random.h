/**
 * Random number generator.
 * by billjeff
 *
 * Revision History:
 *     Jan/06/2011 - Init
 */

#ifndef CLPIM_COMMON_RANDOM_H
#define CLPIM_COMMON_RANDOM_H

#include "CommonInclude.h"

namespace CLPIM
{

class Random
{
public:
    /**
     * Get next random int
     */
    static int32 NextInt();

private:
    static bool mInitFlag;
};

} /// namespace CLPIM

#endif /// CLPIM_COMMON_RANDOM_H
