#include <pthread.h>
#include <stdio.h>

int pthread_create(pthread_t*, const pthread_attr_t*, void* (*)(void*), void*)
    __attribute__((weak));

// When an ELF linker sees a weak reference, it does not extract an archive member to satisfy the
// weak reference.
int main(void) {
  if (pthread_create) {
    printf("This is multi-thread version!\n");
  } else {
    printf("This is single-thread version!\n");
  }
}
