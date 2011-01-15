/**
 * The following codes are picked out from Linux atomic.h
 * TODO: How to use Linux's atomic.h instead of this one? 
 *       I have removed LOCK_PREFIX macro.
 */

//#include <bits/atomicity.h>


/**
 * TODO: Test LOCK_PREFIX on single core machine.
 */
#define LOCK_PREFIX ""

/*
#define LOCK_PREFIX                   \
    ".section .smp_locks,\"a\"\n"     \
    "  .align 8\n"                    \
    "  .quad 661f\n"                  \
    ".previous\n"                     \
    "661:\n\tlock; "
*/

typedef struct { volatile int counter; } atomic_t;

#define ATOMIC_INIT(i)	{ (i) }

#define atomic_read(v)		((v)->counter)

#define atomic_set(v,i)		(((v)->counter) = (i))

static __inline__ void atomic_inc(atomic_t *v)
{
	__asm__ __volatile__(
		LOCK_PREFIX "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * Decrease counter by 1, return true if counter is zero.
 */
static __inline__ int atomic_dec_and_test(atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK_PREFIX "decl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}


/**
 * THE FOLLOWING CODES ARE REFERENCED FROM BOOST LIB
 */

/*
//#if defined(__GLIBCXX__) // g++ 3.4+
using __gnu_cxx::__atomic_add;
using __gnu_cxx::__exchange_and_add;
//#endif

class atomic_count
{
public:
    explicit atomic_count(long v) : value_(v) {}

    void operator++()
    {
        __atomic_add(&value_, 1);
    }

    long operator--()
    {
        return __exchange_and_add(&value_, -1) - 1;
    }

    operator long() const
    {
        return __exchange_and_add(&value_, 0);
    }

private:
    atomic_count(atomic_count const &);
    atomic_count & operator=(atomic_count const &);

    mutable _Atomic_word value_;
};
*/
