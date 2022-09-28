#include <iostream>
#include <string>

struct Type {
  int i = 3;

  Type() {}

  void f(int v) const {
    // ERROR
    // i = v;
    const_cast<Type*>(this)->i = v;
  }
};

void test_const_cast() {
  const char* pc = "hello";
  [[maybe_unused]] auto p = const_cast<char*>(pc);  // OK: but writting through p is undefined
  [[maybe_unused]] std::string s =
      static_cast<std::string>(pc);  // OK: converts string literal to string

  // ERROR: static_cast can't cast away const
  // static_cast<char*>(pc);
  // ERROR: const_cast only changes constness
  // const_cast<std::string>(pc);

  [[maybe_unused]] const int n = 10;
  // ERROR: Const_cast to 'int', which is not a reference, pointer-to-object, or
  // pointer-to-data-member
  // const_cast<int>(n);

  [[maybe_unused]] void (Type::*pmf)(int) const = &Type::f;
  // const_cast<void (Type::*)(int)>(pmf);

  Type t;
  t.f(4);  // if this was const Type t, then t.f(4) would be undefined behavior
  std::cout << "type::i = " << t.i << std::endl;
  const int Type::*pdm = &Type::i;
  t.*const_cast<int Type::*>(pdm) = 5;  // OK
  std::cout << "type::i = " << t.i << std::endl;
}

int main() {
  test_const_cast();
  return 0;
}
