#include "shared_counter.h"

//#define DEBUG_SHARED_COUNTER

#ifdef DEBUG_SHARED_COUNTER
#include <iostream>
using namespace std;
#endif

SharedCounter::SharedCounter(int i)
{
    atomic_set(&mCounter, i);
}

void SharedCounter::Inc()
{
    atomic_inc(&mCounter);
#ifdef DEBUG_SHARED_COUNTER
    cout << "counter: " << atomic_read(&mCounter) << endl;
#endif
}

bool SharedCounter::DecAndTest()
{
#ifdef DEBUG_SHARED_COUNTER
    int ret = atomic_dec_and_test(&mCounter);
    cout << "after dec, counter: " << atomic_read(&mCounter) << endl;
    cout << "dec and test ret: " << ret << endl;
    return ret;
#else
    return atomic_dec_and_test(&mCounter);
#endif
}

long SharedCounter::GetCount()
{
    return atomic_read(&mCounter);
}
