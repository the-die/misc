#include <cassert>
#include <cstdint>
#include <ios>
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

int f() { return 42; }

void test_reinterpret_cast() {
  int i = 7;

  // pointer to integer and back
  auto v1 = reinterpret_cast<std::uintptr_t>(&i);
  // ERROR: static_cast is an error
  // auto v = static_cast<std::uintptr_t>(&i);
  // ERROR: cast from 'int*' to 'int' loses precision
  // auto v = reinterpret_cast<int>(&i);
  // ERROR: static_cast from 'int *' to 'int' is not allowed
  // atuo v = static_cast<int>(&i);
  [[maybe_unused]] auto v2 = reinterpret_cast<std::intptr_t>(&i);  // OK
  std::cout << "The value of &i is " << std::showbase << std::hex << v1 << std::endl;
  auto p1 = reinterpret_cast<int*>(v1);
  assert(p1 == &i);

  // pointer to function to another and back
  void (*fp1)() = reinterpret_cast<void (*)()>(f);
  // WARNING: undefined behavior
  // fp1();
  int (*fp2)() = reinterpret_cast<int (*)()>(fp1);
  std::cout << std::dec << fp2() << std::endl;  // safe

  // type aliasing through pointer
  char* p2 = reinterpret_cast<char*>(&i);
  std::cout << (p2[0] == '\x7' ? "This system is little-endian\n" : "This system is big-endian\n");

  // type aliasing through reference
  reinterpret_cast<unsigned int&>(i) = 42;
  std::cout << i << std::endl;

  [[maybe_unused]] const int& const_iref = i;
  // ERROR: can't get rid of const
  // int &iref = reinterpret_cast<int&>(const_iref);
  [[maybe_unused]] int& iref = const_cast<int&>(const_iref);  // OK
}

int main() {
  test_const_cast();
  test_reinterpret_cast();
  return 0;
}
