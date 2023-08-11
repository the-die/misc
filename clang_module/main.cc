#include "B.h"

int main() {
  std::vector<std::string> v{"hello", "world"};
  auto print = [](const std::string& s) { std::cout << s << ' '; };
  std::for_each(v.cbegin(), v.cend(), print);

#ifdef ENABLE_A
  a(123);
#endif

  return 0;
}
