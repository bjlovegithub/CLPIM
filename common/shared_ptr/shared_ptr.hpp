#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include "shared_counter.h"

#include <cassert>

#define USE_ATOMIC_T

namespace CLPIM
{

template <class T>
class SharedPtr
{
public:
    /**
     * Constructor, initialize an emtpy shared pointer.
     */
    explicit SharedPtr() : mPtr(0)
    {
#ifdef USE_ATOMIC_T
        mCounter = new SharedCounter(0);
#else
        mCounter = new atomic_count(0);
#endif
    }

    /**
     * Constructor, initialize shared pointer with a pointer.
     * @p Pointer to type T.
     */
    explicit SharedPtr(T *p) : mPtr(p)
    {
#ifdef USE_ATOMIC_T
        mCounter = new SharedCounter(1);
#else
        mCounter = new atomic_count(1);
#endif
    }

    /**
     * Destructor, if counter is equalling to zero, delete pointer.
     */
    ~SharedPtr()
    {
#ifdef USE_ATOMIC_T
        if (mCounter->DecAndTest())
#else
        if (!(--(*mCounter)))
#endif
        {
            delete mPtr;
            delete mCounter;
        }
    }

    /**
     * Copy constructor, it will transfer the owned pointer to destinate
     * shared pointer, and increase counter.
     * @ptr Pointer to source shared pointer.
     */
    SharedPtr(const SharedPtr &ptr)
    {
        mPtr = ptr.mPtr;
        mCounter = ptr.mCounter;
#ifdef USE_ATOMIC_T
        mCounter->Inc();
#else
        mCounter->operator++();
#endif
    }

    /**
     * Assignment operator, transfer shared pointer and increase counter.
     */
    SharedPtr& operator=(const SharedPtr &ptr)
    {
        if (this == &ptr)
            return *this;
        /// if these two pointers refer to different objects.
        if (mPtr != ptr.mPtr)
        {
#ifdef USE_ATOMIC_T
            if (mCounter->DecAndTest())
#else
            if (!(mCounter->operator--()))
#endif
            {
                delete mPtr;
                delete mCounter;
            }
            mPtr = ptr.mPtr;
            mCounter = ptr.mCounter;
#ifdef USE_ATOMIC_T
            mCounter->Inc();
#else
            mCounter->operator++();
#endif
        }
        else
        {
            assert(mCounter == ptr.mCounter);
#ifdef USE_ATOMIC_T
            mCounter->Inc();
#else
            mCounter->operator++();
#endif
        }

        return *this;
    }

    /**
     * Return true if mPtr is not NULL.
     */
    operator bool()
    {
        return (mPtr != 0);
    }

    /**
     * Call T's member function and visit data member.
     */
    T* operator->()
    {
        return mPtr;
    }

    T& operator*()
    {
        assert(mPtr);
        return *mPtr;
    }

    const T& operator*() const
    {
        return *mPtr;
    }

    /**
     * Get raw pointer of T
     */
    T* Get()
    {
        return mPtr;
    }

    /**
     * Reset the raw ptr(mPtr).
     */
    void Reset(T *ptr)
    {
        delete mPtr;
        mPtr = ptr;
        delete mCounter;
#ifdef USE_ATOMIC_T
        mCounter = new SharedCounter(1);
#else
        mCounter = new atomic_count(1);
#endif
    }

    /**
     * Test whether the counter is 1.
     */
    bool Unique()
    {
        long left;
#ifdef USE_ATOMIC_T
        left = mCounter->GetCount();
#else
        left = *mCounter;
#endif
        return (left == 1);
    }

private:
    T* mPtr;
#ifdef USE_ATOMIC_T
    SharedCounter *mCounter;
#else
    atomic_count *mCounter;
#endif
};

} /// namespace CLPIM

#endif // SHARED_PTR_HPP
