// Clang does not automatically use PCH files for headers that are directly included within a source
// file.
int main() {
  int n = 5;
  auto v = test1<3>("hello");
  auto print = [](const std::string& s) { std::cout << s << ' '; };
  std::cout << "start:\t";
  std::for_each(v.cbegin(), v.cend(), print);
  std::cout << '\n';
  test2(n);
  return 0;
}
