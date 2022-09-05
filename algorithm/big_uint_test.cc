#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "big_uint.h"

BigUint<> Factorial(int n) {
  if (n == 1) return BigUint<>(1);
  return BigUint<>(n) * Factorial(n - 1);
}

int main() {
  std::stringstream ss;
  ss << Factorial(100);
  std::string n1 = ss.str();
  std::string n2 =
      "93326215443944152681699238856266700490715968264381621468592963895217599993229915608941463976"
      "156518286253697920827223758251185210916864000000000000000000000000";

  assert(n1 == n2);
  std::cout << "OK" << std::endl;
  return 0;
}
