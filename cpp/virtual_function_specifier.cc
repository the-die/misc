/*
 * virtual funciton specifier
 * 虚函数说明符
 *
 * The `virtual` specifier specifies that a non-static member function is
 * virtual and supports dynamic dispatch. It may only appear in the
 * `decl-specifier-seq` of the initial declaration of a non-static member
 * function (i.e., when it is declared in the class definition).
 */

/*
 * Virtual functions are member functions whose behavior can be overridden in
 * derived classes. As opposed to non-virtual functions, the overriding behavior
 * is preserved even if there is no compile-time information about the actual
 * type of the class. That is to say, if a derived class is handled using
 * pointer or reference to the base class, a call to an overridden virtual
 * function would invoke the behavior defined in the derived class. Such a
 * function call is known as virtual function call or virtual call. Virtual
 * function call is suppressed if the function is selected using qualified name
 * lookup (that is, if the function's name appears to the right of the scope
 * resolution operator ::).
 */

/*
#include <iostream>

struct Base {
  virtual void f() { std::cout << "base\n"; }
};

struct Derived : Base {
  void f() override  // 'override' is optional
  {
    std::cout << "derived\n";
  }
};

int main() {
  Base b;
  Derived d;

  // virtual function call through reference
  Base& br = b;  // the type of br is Base&
  Base& dr = d;  // the type of dr is Base& as well
  br.f();        // prints "base"
  dr.f();        // prints "derived"

  // virtual function call through pointer
  Base* bp = &b;  // the type of bp is Base*
  Base* dp = &d;  // the type of dp is Base* as well
  bp->f();        // prints "base"
  dp->f();        // prints "derived"

  // non-virtual function call
  br.Base::f();  // prints "base"
  dr.Base::f();  // prints "base"
}
*/

/*
 * If some member function vf is declared as virtual in a class Base, and some
 * class Derived, which is derived, directly or indirectly, from Base, has a
 * declaration for member function with the same
 *    name
 *    parameter type list (but not the return type)
 *    cv-qualifiers
 *    ref-qualifiers
 *
 * Then this function in the class Derived is also virtual (whether or not the
 * keyword virtual is used in its declaration) and overrides Base::vf (whether
 * or not the word override is used in its declaration).
 *
 * Base::vf does not need to be accessible or visible to be overridden.
 * (Base::vf can be declared private, or Base can be inherited using private
 * inheritance. Any members with the same name in a base class of Derived which
 * inherits Base do not matter for override determination, even if they would
 * hide Base::vf during name lookup.)
 */

/*
class B {
  virtual void do_f();  // private member
 public:
  void f() { do_f(); }  // public interface
};

struct D : public B {
  void do_f() override;  // overrides B::do_f
};

int main() {
  D d;
  B* bp = &d;
  bp->f();  // internally calls D::do_f();
}
*/

/*
 * For every virtual function, there is the final overrider, which is executed
 * when a virtual function call is made. A virtual member function vf of a base
 * class Base is the final overrider unless the derived class declares or
 * inherits (through multiple inheritance) another function that overrides vf.
 */

/*
struct A {
  virtual void f();
};  // A::f is virtual
struct B : A {
  void f();
};  // B::f overrides A::f in B
struct C : virtual B {
  void f();
};  // C::f overrides A::f in C

struct D : virtual B {
};  // D does not introduce an overrider, B::f is final in D

struct E : C,
           D  // E does not introduce an overrider, C::f is final in E
{
  using A::f;  // not a function declaration, just makes A::f visible to lookup
};

int main() {
  E e;
  e.f();     // virtual call calls C::f, the final overrider in e
  e.E::f();  // non-virtual call calls A::f, which is visible in E
}
*/

// If a function has more than one final overrider, the program is ill-formed:

/*
struct A {
  virtual void f();
};

struct VB1 : virtual A {
  void f();  // overrides A::f
};

struct VB2 : virtual A {
  void f();  // overrides A::f
};

// struct Error : VB1, VB2
// {
//     // Error: A::f has two final overriders in Error
// };

struct Okay : VB1, VB2 {
  void f();  // OK: this is the final overrider for A::f
};

struct VB1a : virtual A {};  // does not declare an overrider

struct Da : VB1a, VB2 {
  // in Da, the final overrider of A::f is VB2::f
};
*/

/*
 * A function with the same name but different parameter list does not override
 * the base function of the same name, but hides it: when unqualified name
 * lookup examines the scope of the derived class, the lookup finds the
 * declaration and does not examine the base class.
 */

/*
struct B {
  virtual void f();
};

struct D : B {
  void f(int);  // D::f hides B::f (wrong parameter list)
};

struct D2 : D {
  void f();  // D2::f overrides B::f (doesn't matter that it's not visible)
};

int main() {
  B b;
  B& b_as_b = b;

  D d;
  B& d_as_b = d;
  D& d_as_d = d;

  D2 d2;
  B& d2_as_b = d2;
  D& d2_as_d = d2;

  b_as_b.f();   // calls B::f()
  d_as_b.f();   // calls B::f()
  d2_as_b.f();  // calls D2::f()

  d_as_d.f();   // Error: lookup in D finds only f(int)
  d2_as_d.f();  // Error: lookup in D finds only f(int)
}
*/
