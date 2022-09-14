#ifndef ATOMIC_VAR_H_
#define ATOMIC_VAR_H_

#include <pthread.h>

#if (__i386 || __amd64 || __powerpc__) && __GNUC__
#  define GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#  if defined(__clang__)
#    define HAVE_ATOMIC
#  endif
#  if (defined(__GLIBC__) && defined(__GLIBC_PREREQ))
#    if (GNUC_VERSION >= 40100 && __GLIBC_PREREQ(2, 6))
#      define HAVE_ATOMIC
#    endif
#  endif
#endif

// ATOMIC_INCR(var, count, mutex) -- Increment the atomic var
// ATOMIC_DECR(var, count, mutex) -- Decrement the atomic var
// ATOMIC_GET(var, dstvar, mutex) -- Fetch the atomic var value
// Never use return value from the macros. To update and get use instead:
//  ATOMIC_INCR(var, ...);
//  ATOMIC_GET(var , dstvar, ...);
//  DoSomethingWith(dstvar);

// using __atomic macros
// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
#if defined(__ATOMIC_RELAXED)

#  define ATOMIC_INCR(var, count, mutex) __atomic_add_fetch(&var, (count), __ATOMIC_RELAXED)
#  define ATOMIC_DECR(var, count, mutex) __atomic_sub_fetch(&var, (count), __ATOMIC_RELAXED)
#  define ATOMIC_GET(var, dstvar, mutex)                \
    do {                                                \
      dstvar = __atomic_load_n(&var, __ATOMIC_RELAXED); \
    } while (0)

// using __sync macros
// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fsync-Builtins.html
#elif defined(HAVE_ATOMIC)

#  define ATOMIC_INCR(var, count, mutex) __sync_add_and_fetch(&var, (count))
#  define ATOMIC_DECR(var, count, mutex) __sync_sub_and_fetch(&var, (count))
#  define ATOMIC_GET(var, dstvar, mutex)      \
    do {                                      \
      dstvar = __sync_sub_and_fetch(&var, 0); \
    } while (0)

// using pthread mutex
#else

#  define ATOMIC_INCR(var, count, mutex) \
    do {                                 \
      pthread_mutex_lock(&mutex);        \
      var += (count);                    \
      pthread_mutex_unlock(&mutex);      \
    } while (0)
#  define ATOMIC_DECR(var, count, mutex) \
    do {                                 \
      pthread_mutex_lock(&mutex);        \
      var -= (count);                    \
      pthread_mutex_unlock(&mutex);      \
    } while (0)
#  define ATOMIC_GET(var, dstvar, mutex) \
    do {                                 \
      pthread_mutex_lock(&mutex);        \
      dstvar = var;                      \
      pthread_mutex_unlock(&mutex);      \
    } while (0)

#endif

#endif  // ATOMIC_VAR_H_
