// weakref
// weakref ("target")
// The weakref attribute marks a declaration as a weak reference. Without arguments, it should be
// accompanied by an alias attribute naming the target symbol. Optionally, the target may be given
// as an argument to weakref itself. In either case, weakref implicitly marks the declaration as
// weak. Without a target, given as an argument to weakref or to alias, weakref is equivalent to
// weak.
//           static int x() __attribute__ ((weakref ("y")));
//           /* is equivalent to... */
//           static int x() __attribute__ ((weak, weakref, alias ("y")));
//           /* and to... */
//           static int x() __attribute__ ((weakref));
//           static int x() __attribute__ ((alias ("y")));
//
// A weak reference is an alias that does not by itself require a definition to be given for the
// target symbol. If the target symbol is only referenced through weak references, then it becomes a
// weak undefined symbol. If it is directly referenced, however, then such strong references
// prevail, and a definition will be required for the symbol, not necessarily in the same
// translation unit.
//
// The effect is equivalent to moving all references to the alias to a separate translation unit,
// renaming the alias to the aliased symbol, declaring it as weak, compiling the two separate
// translation units and performing a reloadable link on them.
//
// At present, a declaration to which weakref is attached can only be static.

#include <stdio.h>

static int x1 __attribute__((weakref("y1")));
static int x2 __attribute__((weakref("y2")));
static int x3 __attribute__((weakref("y3")));

// [gcc] error: ‘x’ defined both normally and as ‘alias’ attribute
// [clang] error: weakref declaration must have internal linkage
// int x __attribute__((weakref("y")));

static void funx1(void) __attribute__((weakref("funy1")));
static void funx2(void) __attribute__((weakref("funy2")));

// [gcc] error: ‘weakref’ symbol ‘funx’ must have static linkage
// [clang] error: weakref declaration must have internal linkage
// void funx(void) __attribute__((weakref("funy")));

int main(void) {
  printf("[info] x1 address is %p\n", &x1);
  printf("[info] x2 address is %p\n", &x2);
  printf("[info] x2 value is %d\n", x2);
  printf("[info] x3 value is %d\n", *(int*)&x3);
  printf("[info] x3 value is %f\n", *(double*)&x3);
  printf("[info] funx1 address is %p\n", &funx1);
  funx2();
  return 0;
}
