#include <iostream>
#include <boost/shared_ptr.hpp>
#include "atomic.h"

using namespace std;
using namespace boost;

const int ThreadNum = 10;
const int THREADNUM = 1000;

#define TEST_ATOMIC_T

class Foo
{
public:
    Foo()
    {
        cout << "Ctor" << endl;
    }
    
    ~Foo()
    {
        cout << "Dtor" << endl;
    }

    int Get()
    {
        return 1;
    }
};

shared_ptr<Foo> globalPtr;

void bar(shared_ptr<Foo> ptr)
{
    int i = ptr->Get();
    assert(i == 1);
}

void* thread(void *arg)
{
    for (int i = 0; i < 10000; ++i)
        bar(globalPtr);
    pthread_exit(0);
}

#ifdef TEST_ATOMIC_COUNT
atomic_count counter(0);
//long counter = 0;

void* inc_thread(void *arg)
{
    for (int i = 0; i < 10000; ++i)
        ++counter;
    pthread_exit(0);
}

void* dec_thread(void *arg)
{
    for (int i = 0; i < 10000; ++i)
        --counter;
    pthread_exit(0);
}
#endif

#ifdef TEST_ATOMIC_T
atomic_t counter;
//long counter = 0;

void* inc_thread(void *arg)
{
    for (int i = 0; i < 10000; ++i)
        atomic_inc(&counter);
    pthread_exit(0);
}

void* dec_thread(void *arg)
{
    for (int i = 0; i < 10000; ++i)
        atomic_dec_and_test(&counter);
    pthread_exit(0);
}
#endif

int main(void)
{
    globalPtr.reset(new Foo);

    /// create 100 threads to test shared_ptr is OK or not.
    pthread_t threadIDs[ThreadNum];
    for (int i = 0; i < ThreadNum; ++i)
    {
        pthread_create(&threadIDs[i], NULL, thread, (void*)NULL);
    }
    
    for (int i = 0; i < ThreadNum; ++i)
        pthread_join(threadIDs[i], NULL);

    cout << "I am going out now..." << endl;

/*
    atomic_set(&counter, 0);
    pthread_t incThreads[THREADNUM], decThreads[THREADNUM];
    for (int i = 0; i < THREADNUM; ++i)
    {
        pthread_create(&incThreads[i], NULL, inc_thread, (void*)NULL);      
        pthread_create(&decThreads[i], NULL, dec_thread, (void*)NULL);      
    }

    for (int i = 0; i < THREADNUM; ++i)
    {
        pthread_join(incThreads[i], NULL);
        pthread_join(decThreads[i], NULL);
    }

    cout << atomic_read(&counter) << endl;
*/
    return 0;
}

