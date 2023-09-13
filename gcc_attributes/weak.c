/*
 * weak
 *
 * The weak attribute causes the declaration to be emitted as a weak symbol rather than a global.
 * This is primarily useful in defining library functions which can be overridden in user code,
 * though it can also be used with non-function declarations. Weak symbols are supported for ELF
 * targets, and also for a.out targets when using the GNU assembler and linker.
 */

#include <stdio.h>

extern int extern_weak_var __attribute__((weak));

int weak_var __attribute__((weak)) = 5;

// error: conflicting types for ‘weak_var’; have ‘double’
// note: previous declaration of ‘weak_var’ with type ‘int’
// double weak_var __attribute__((weak));

extern void weak_fun(void) __attribute__((weak));

extern void weak_fun1(void) __attribute__((weak));

int main(void) {
  printf("[info]: extern_weak_var address is %p\n", &extern_weak_var);
  printf("[info]: weak_var: %d\n", *(int*)&weak_var);
  printf("[info]: weak_var: %f\n", *(double*)&weak_var);
  printf("[info]: weak_fun address is %p\n", &weak_fun);
  if (weak_fun1) weak_fun1();
  return 0;
}
