#include <utility>

// template <class T>
// typename add_rvalue_reference<T>::type declval() noexcept;
//
// Declaration value
//
// Returns an rvalue reference to type T without referring to any object.
//
// This function shall only be used in unevaluated operands (such as the operands of sizeof and
// decltype).
//
// T may be an incomplete type.
//
// This is a helper function used to refer to members of a class in unevaluated operands, especially
// when either the constructor signature is unknown or when no objects of that type can be
// constructed (such as for abstract base classes).

struct A {
  virtual int value() = 0;
};

class B : public A {
  int val_;

 public:
  B(int i, int j) : val_(i * j) {}
  int value() { return val_; }
};

struct Default {
  int foo() const { return 1; }
};

struct NonDefault {
  NonDefault() = delete;
  int foo() const { return 1; }
};

int main() {
  [[maybe_unused]] decltype(std::declval<A>().value()) a;  // int a

  // ERROR: Allocationg an object of abstract class type 'A'
  // decltype(A().value()) a1;

  [[maybe_unused]] decltype(std::declval<B>().value()) b;  // int b

  // ERROR: No matching constructor for initialization of 'B'
  // decltype(B().value()) b1;

  [[maybe_unused]] decltype(B(0, 0).value()) c;  // int c

  decltype(Default().foo()) n1 = 1;  // int n1

  // ERROR: no default constructor
  // decltype(NonDefault().foo()) n2 = n1;
  [[maybe_unused]] decltype(std::declval<NonDefault>().foo()) n2 = n1;  // int n2

  return 0;
}
