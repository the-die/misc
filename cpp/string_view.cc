#include <cstdlib>
#include <iostream>
#include <string_view>

void* operator new(std::size_t cnt) {
  std::cout << "Allocate: " << cnt << " bytes" << std::endl;
  return malloc(cnt);
}

void operator delete(void* p) {
  std::cout << "Free: " << p << std::endl;
  free(p);
}

void operator delete(void*, std::size_t) {}

int main() {
  // These operators are declared in the namespace std::literals::string_view_literals, where both
  // literals and string_view_literals are inline namespaces. Access to these operators can be
  // gained with using namespace std::literals, using namespace std::string_view_literals, and using
  // namespace std::literals::string_view_literals.
  // std::literals::string_view_literals::operator""sv
  // constexpr std::string_view operator "" sv( const char* str, std::size_t len ) noexcept;
  using namespace std::literals;

  std::cout << "---------------------------------------------------------------------" << std::endl;
  std::string_view s1 = "abc\0\0def";
  std::string_view s2 = "abc\0\0def"sv;
  std::string_view s3("abc\0\0def", 6);
  std::cout << "s1: " << s1.size() << " \"" << s1 << "\"\n";
  std::cout << "s2: " << s2.size() << " \"" << s2 << "\"\n";
  std::cout << "s3: " << s3.size() << " \"" << s3 << "\"\n";

  std::cout << "---------------------------------------------------------------------" << std::endl;
  // "Good" case: `good` points to a static array. String literals usually reside in persistent data
  // segments.
  std::string_view good{"a string literal"};
  std::cout << good << std::endl;

  std::cout << "---------------------------------------------------------------------" << std::endl;
  // "Bad" case: `bad` holds a dangling pointer since the std::string temporary, created by
  // std::operator""s, will be destroyed at the end of the statement.
  // std::literals::string_literals::operator""s
  // std::string operator""s(const char *str, std::size_t len);
  // returns std::string{str, len}
  std::string_view bad{"a temporary string"s};
  // ERROR: heap-use-after-free
  std::cout << bad << std::endl;
}
