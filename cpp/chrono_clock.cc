#include <chrono>
#include <iomanip>
#include <iostream>
#include <ratio>

// Class std::chrono::system_clock represents the system-wide real time wall clock.
// Class std::chrono::steady_clock represents a monotonic clock.
// Class std::chrono::high_resolution_clock represents the clock with the smallest tick period
// provided by the implementation.
template <typename T>
void Test() {
  std::cout << "\tprecision: ";
  // a std::ratio type representing the tick period of the clock, in seconds
  typedef typename T::period P;
  if (std::ratio_less_equal<P, std::milli>::value) {
    typedef typename std::ratio_multiply<P, std::kilo>::type TT;
    std::cout << std::fixed << double(TT::num) / TT::den << " milliseconds" << std::endl;
  } else {
    std::cout << std::fixed << double(P::num) / P::den << " seconds" << std::endl;
  }
  // true if the time between ticks is always constant, i.e. calls to now() return values that
  // increase monotonically even in case of some external clock adjustment, otherwise false
  std::cout << "\tis_steady: " << std::boolalpha << T::is_steady << std::endl;
}

int main() {
  std::cout << "system_clock: " << std::endl;
  Test<std::chrono::system_clock>();
  std::cout << "\nhigh_resolution_clock: " << std::endl;
  Test<std::chrono::high_resolution_clock>();
  std::cout << "\nsteady_clock: " << std::endl;
  Test<std::chrono::steady_clock>();

  return 0;
}
