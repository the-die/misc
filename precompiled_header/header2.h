#pragma once

#include <iostream>

template <typename T>
void test2(const T& e) {
  std::cout << "[test]: " << e << "\n";
}
