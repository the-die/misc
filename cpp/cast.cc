#include <cassert>
#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <string>

#include <cxxabi.h>

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

class A {
 public:
  A(int8_t a) : a_(a) {}
  virtual void Print() { std::cout << "A::Print" << std::endl; }
  virtual ~A() {}
  int8_t a_;
};

class AA {
 public:
  AA(int8_t a) : a_(a) {}
  int8_t a_;
};

class B {
 public:
  B(int16_t b) : b_(b) {}
  virtual void Show() { std::cout << "B::Show" << std::endl; }
  virtual ~B() {}
  int16_t b_;
};

class C {
 public:
  C(int32_t c) : c_(c) {}
  virtual void Display() { std::cout << "C::Display" << std::endl; }
  virtual ~C() {}
  int32_t c_;
};

class D : public A, public B, public C {
 public:
  D(int8_t a, int16_t b, int32_t c) : A(a), B(b), C(c) {}
  void Print() override { std::cout << "D::Print" << std::endl; }
  void Show() override { std::cout << "D::Show" << std::endl; }
  void Display() override { std::cout << "D::Display" << std::endl; }
  virtual ~D() {}
  int64_t d_;
};

// 'static_cast' can perform conversions between pointers to related classes, not only upcasts (from
// pointer-to-derived to pointer-to-base), but also downcasts (from pointer-to-base to
// pointer-to-derived). No checks are performed during runtime to guarantee that the object being
// converted is in fact a full object of the destination type. Therefore, it is up to the programmer
// to ensure that the conversion is safe. On the other side, it does not incur the overhead of the
// type-safety checks of 'dynamic_cast'.
void test_static_cast() {
  std::cout << "sizeof(A): " << sizeof(A) << std::endl;
  std::cout << "sizeof(AA): " << sizeof(AA) << std::endl;
  std::cout << "sizeof(B): " << sizeof(B) << std::endl;
  std::cout << "sizeof(C): " << sizeof(C) << std::endl;
  std::cout << "sizeof(D): " << sizeof(D) << std::endl;

  std::cout << "is_standard_layout_v<A>: " << std::boolalpha
            << std::is_standard_layout_v<A> << std::endl;
  std::cout << "is_standard_layout_v<AA>: " << std::is_standard_layout_v<AA> << std::noboolalpha
            << std::endl;

  A a(100);
  D d(1, 2, 3);
  auto p1 = &a;
  auto p2 = &a.a_;
  std::unique_ptr<char, decltype(&std::free)> name1{
      abi::__cxa_demangle(typeid(p1).name(), nullptr, nullptr, nullptr), std::free};
  std::unique_ptr<char, decltype(&std::free)> name2{
      abi::__cxa_demangle(typeid(p2).name(), nullptr, nullptr, nullptr), std::free};
  std::unique_ptr<char, void (*)(void*)> name3{
      abi::__cxa_demangle(typeid(decltype(std::free)).name(), nullptr, nullptr, nullptr),
      std::free};

  std::cout << "p1 type: " << name1.get() << std::endl;
  std::cout << "p2 type: " << name2.get() << std::endl;
  std::cout << "decltype(std::free) type: " << name3.get() << std::endl;

  //         class A
  // b --> +----------+
  //       |  vtable  | 8
  //       +----------+
  //       |    a_    | 1
  //       +----------+
  //       | padding  | 7
  //       +----------+
  std::cout << "address of a: " << &a << std::endl;
  std::cout << "address of a.a_: " << (void*)&a.a_ << std::endl;
  std::cout << "offsetof(AA, a_): " << offsetof(AA, a_) << std::endl;
  //         class D
  // b --> +----------+
  //       |  vtable  | 8
  //       +----------+
  //       |    a_    | 1
  //       +----------+
  //       | padding  | 7
  //       +----------+
  //       |  vtable  | 8
  //       +----------+
  //       |    b_    | 2
  //       +----------+
  //       | padding  | 6
  //       +----------+
  //       |  vtable  | 8
  //       +----------+
  //       |    c_    | 4
  //       +----------+
  //       | padding  | 4
  //       +----------+
  //       |    d_    | 8
  //       +----------+
  std::cout << "address of d: " << &d << std::endl;
  std::cout << "address of d.a_: " << (void*)&d.a_ << std::endl;
  std::cout << "address of d.b_: " << (void*)&d.b_ << std::endl;
  std::cout << "address of d.c_: " << (void*)&d.c_ << std::endl;
  std::cout << "address of d.d_: " << (void*)&d.d_ << std::endl;

  // upcast
  auto p3 = new D(2, 4, 6);
  std::cout << "p3: " << p3 << std::endl;
  std::cout << "static_cast<C*>(p3)" << static_cast<C*>(p3) << std::endl;
  std::cout << "static_cast<B*>(p3)" << static_cast<B*>(p3) << std::endl;
  std::cout << "static_cast<A*>(p3)" << static_cast<A*>(p3) << std::endl;
  delete p3;

  // downcast
  auto p4 = new A(7);
  auto p5 = new B(8);
  auto p6 = new C(9);
  std::cout << "p4: " << p4 << std::endl;
  std::cout << "p5: " << p5 << std::endl;
  std::cout << "p6: " << p6 << std::endl;
  std::cout << "static_cast<D*>(p4)" << static_cast<D*>(p4) << std::endl;
  std::cout << "static_cast<D*>(p5)" << static_cast<D*>(p5) << std::endl;
  std::cout << "static_cast<D*>(p6)" << static_cast<D*>(p6) << std::endl;
  delete p4;
  delete p5;
  delete p6;
}

int main() {
  test_const_cast();
  test_reinterpret_cast();
  test_static_cast();
  return 0;
}
