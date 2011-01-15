#ifndef SHARED_COUNTER_H
#define SHARED_COUNTER_H

#include "atomic.h"

class SharedCounter
{
public:
    /// TODO - Test the performance with mutex lock.
    /**
     * Constructor initlize mCounter
     * @i Initial value for counter
     */
    SharedCounter(int i = 0);

    /**
     * Increase counter by 1.
     */
    void Inc();

    /**
     * Decrease counter by 1, return true if counter equals to zero, 
     * return false if it is not.
     */
    bool DecAndTest();

    /**
     * Return the value of mCounter.
     */
    long GetCount();

private:
    atomic_t mCounter;
};

#endif // SHARED_COUNTER_H
