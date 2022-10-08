#include <cassert>
#include <complex>
#include <iostream>

/*
#ifdef NDEBUG
#  define assert(condition) ((void)0)
#else
#  define assert(condition)  // implementation defined
#endif
*/

// =====================================================
//                    assert notes
// =====================================================

// 1. Checking parameters with assert
constexpr int kMaxSize = 100;
void SetSize(int n) {
  assert(n >= 0 && n < kMaxSize);
  // do something
}

// 2. Use assert to check for changes to this function
int Init(int n) {
  int ret = n;
  // do something
  assert(ret >= 0 && ret < kMaxSize);
  return ret;
}

// Use (void) to silence unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

int main() {
  SetSize(10);
  [[maybe_unused]] int n = Init(20);
  assert(2 + 2 == 4);
  std::cout << "Checkpoint #1\n";

  assert((void("void helps to avoid 'unused value' warning"), 2 * 2 == 4));
  std::cout << "Checkpoint #2\n";

  assert((010 + 010 == 16) && "Yet another way to add an assert message");
  std::cout << "Checkpoint #3\n";

  assertm((2 + 2) % 3 == 1, "Expect expected");
  std::cout << "Checkpoint #4\n";

  assertm(2 + 2 == 5, "There are five lights");               // assertion fails
  std::cout << "Execution continues past the last assert\n";  // No

  // ERROR: assert does not take two arguments
  // assert(std::is_same_v<int, int>);
  // OK: one argument
  assert((std::is_same_v<int, int>));
  std::complex<double> c;
  // ERROR
  // assert(c == std::complex<double>{0, 0});
  // OK
  assert((c == std::complex<double>{0, 0}));
}
