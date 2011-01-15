/**
 * TODO: Add multi-thread test case.
 * For increasing shared pointer's counter is atomic, it is
 * safe to copy or assign the same shared pointer in different
 * thread concurrently.
 */

#include "shared_ptr.hpp"

#include <iostream>
#include <cassert>

using namespace std;
using namespace CLPIM;

const int ThreadNum = 1000;

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

SharedPtr<Foo> globalPtr;

void bar(SharedPtr<Foo> ptr)
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


int main(void)
{
    globalPtr.Reset(new Foo);
    assert(globalPtr.Unique() == true);

    cout << "Test Assignment Operator" << endl;
    /// test assignment operator
    {
        SharedPtr<Foo> testPtr(new Foo);
        testPtr = globalPtr;
        assert(!testPtr.Unique());
        assert(!globalPtr.Unique());
    }
    assert(globalPtr.Unique());

    cout << "Test Getter" << endl;
    /// Test reset and Getter
    {
        SharedPtr<Foo> testPtr;
        assert(!testPtr);
        assert(!testPtr.Get());
        testPtr.Reset(new Foo);
        assert(testPtr.Unique() == 1);
        assert(testPtr);
        assert(testPtr.Get());
    }

    cout << "Test Copy Ctor" << endl;
    /// Test copy constructor
    {
        SharedPtr<Foo> testPtr1(new Foo);
        SharedPtr<Foo> testPtr2(testPtr1);
        assert(testPtr1.Unique() != 1);
        assert(testPtr2.Unique() != 1);
    }

    /// create 100 threads to test shared_ptr is OK or not.
    pthread_t threadIDs[ThreadNum];
    for (int i = 0; i < ThreadNum; ++i)
    {
        pthread_create(&threadIDs[i], NULL, thread, (void*)NULL);
    }
    
    for (int i = 0; i < ThreadNum; ++i)
        pthread_join(threadIDs[i], NULL);

    cout << "I am going out now..." << endl;

    return 0;
}
