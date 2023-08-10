#pragma once

#include <string>
#include <vector>

template <int N>
std::vector<std::string> test1(const std::string& s) {
  std::vector<std::string> ret;
  for (int i = 0; i < N; ++i) ret.emplace_back(s);
  return ret;
}
