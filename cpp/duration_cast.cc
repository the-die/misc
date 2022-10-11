#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>

void f() { std::this_thread::sleep_for(std::chrono::seconds(1)); }

// Casting between integer durations where the source period is exactly divisible by the target
// period (e.g. hours to minutes) or between floating-point durations can be performed with ordinary
// casts or implicitly via std::chrono::duration constructors, no duration_cast is needed.
//
// Casting from a floating-point duration to an integer duration is subject to undefined behavior
// when the floating-point value is NaN, infinity, or too large to be representable by the target's
// integer type. Otherwise, casting to an integer duration is subject to truncation as with any
// static_cast to an integer type.

int main() {
  auto t1 = std::chrono::high_resolution_clock::now();
  f();
  auto t2 = std::chrono::high_resolution_clock::now();

  // floating-point duration: no duration_cast needed
  std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

  // integral duration: requires duration_cast
  auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

  // converting integral duration to integral duration of shorter divisible time unit: no
  // duration_cast needed
  std::chrono::duration<long, std::micro> int_usec = int_ms;
  std::cout << "f() took " << fp_ms.count() << " ms, "
            << "or " << int_ms.count() << " whole milliseconds "
            << "(which is " << int_usec.count() << " whole microseconds)" << std::endl;
}
