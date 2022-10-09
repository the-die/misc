#include <iostream>

// =============================================
//             Function declaration
// =============================================

// 1
// noptr-declarator ( parameter-list ) cv(optional) ref(optional) except(optional) attr(optional)

// 2
// noptr-declarator ( parameter-list ) cv(optional) ref(optional) except(optional) attr(optional) ->
// trailing

// @noptr-declarator
// Any valid declarator, but if it begins with *, &, or &&, it has to be surrounded by parentheses.

// =============================================
//            Return type deduction
// =============================================

// If the decl-specifier-seq of the function declaration contains the keyword auto, trailing return
// type may be omitted, and will be deduced by the compiler from the type of the expression used in
// the return statement. If the return type does not use decltype(auto), the deduction follows the
// rules of template argument deduction.
namespace a {

int x = 1;
auto f() { return x; }         // return type is int
const auto& g() { return x; }  // return type is const int&

};  // namespace a

// If the return type is decltype(auto), the return type is as what would be obtained if the
// expression used in the return statement were wrapped in decltype.
// (note: "const decltype(auto)&" is an error, decltype(auto) must be used on its own)
namespace b {

int x = 1;
decltype(auto) f() { return x; }    // return type is int, same as decltype(x)
decltype(auto) g() { return (x); }  // return type is int&, same as decltype((x))
// ERROR: 'decltype(auto)' cannot be cv-qualified
// const decltype(auto) g() { return (x); }

};  // namespace b

// If there are multiple return statements, they must all deduce to the same type.
// ERROR
/*
auto f(bool val) {
  if (val)
    return 123;  // deduces return type int
  else
    return 3.14f;  // error: deduces return type float
}
*/

// If there is no return statement or if the argument of the return statement is a void expression,
// the declared return type must be either decltype(auto), in which case the deduced return type is
// void, or (possibly cv-qualified) auto, in which case the deduced return type is then (identically
// cv-qualified) void.
namespace c {

auto f() {}               // returns void
auto g() { return f(); }  // returns void
decltype(auto) h() {}     // returns void

// ERROR: cannot deduce auto* from void
// auto* x() {}

};  // namespace c

// Once a return statement has been seen in a function, the return type deduced from that statement
// can be used in the rest of the function, including in other return statements.
auto sum(int i) {
  if (i == 1)
    return i;  // sum’s return type is int
  else
    return sum(i - 1) + i;  // okay: sum’s return type is already known
}

// If the return statement uses a brace-init-list, deduction is not allowed.
// ERROR: returning initializer list
// auto func () { return {1, 2, 3}; }

// Virtual functions and coroutines(since C++20) cannot use return type deduction.
// ERROR: virtual function cannot have deduced return type
/*
struct F {
  virtual auto f() { return 2; }
};
*/

// Function templates other than user-defined conversion functions can use return type deduction.
// The deduction takes place at instantiation even if the expression in the return statement is not
// dependent. This instantiation is not in an immediate context for the purposes of SFINAE.
namespace d {

template <class T>
auto f(T t) {
  std::cout << "f(T t)" << std::endl;
  return t;
}
typedef decltype(f(1)) fint_t;  // instantiates f<int> to deduce return type

template <class T>
auto f(T* t) {
  std::cout << "f(T* t)" << std::endl;
  return *t;
}

void g() {
  int (*p)(int*) = &f;
  p(nullptr);
}  // instantiates both fs to determine return types, chooses second template overload

}  // namespace d

// Redeclarations or specializations of functions or function templates that use return type
// deduction must use the same return type placeholders.
namespace e {

auto f(int num) { return num; }
// ERROR: no placeholder return type
// int f(int num);
// ERROR: different placeholder
// decltype(auto) f(int num);

template <typename T>
auto g(T t) {
  return t;
}

template auto g(int);  // okay: return type is int

// ERROR: not a specialization of the primary template g
// template char g(char);

}  // namespace e

// Similarly, redeclarations or specializations of functions or function templates that do not use
// return type deduction must not use a placeholder.
namespace f {

int f(int num);
// ERROR: not a redeclaration of f
// auto f(int num) { return num; }

template <typename T>
T g(T t) {
  return t;
}

template int g(int);  // okay: specialize T as int

// ERROR: not a specialization of the primary template g
// template auto g(char);

}  // namespace f

// Explicit instantiation declarations do not themselves instantiate function templates that use
// return type deduction.
namespace g {

template <typename T>
auto f(T t) {
  return t;
}

// ERROR: does not instantiate f<int>
// extern template auto f(int);

int (*p)(int) = f;  // instantiates f<int> to determine its return type, but an explicit
                    // instantiation definition is still required somewhere in the program

}  // namespace g

int main() {
  d::g();
  return 0;
}
