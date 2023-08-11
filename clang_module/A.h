#pragma once

#ifdef ENABLE_A
#  include <iostream>

template <typename T>
void a(const T& e) {
  std::cout << e << "\n";
}
#endif
