// =========================================================================
//                            value categories
// =========================================================================

// The C++17 standard defines expression value categories as follows:
//
//   * A glvalue is an expression whose evaluation determines the identity of an object, bit-field,
//     or function.
//   * A prvalue is an expression whose evaluation initializes an object or a bit-field, or computes
//     the value of the operand of an operator, as specified by the context in which it appears.
//   * An xvalue is a glvalue that denotes an object or bit-field whose resources can be reused
//     (usually because it is near the end of its lifetime). Example: Certain kinds of expressions
//     involving rvalue references (8.3.2) yield xvalues, such as a call to a function whose return
//     type is an rvalue reference or a cast to an rvalue reference type.
//   * An lvalue is a glvalue that is not an xvalue.
//   * An rvalue is a prvalue or an xvalue.

//          expression
//              |
//         +----+-----+
//         |          |
//      glvalue     rvalue
//         |          |
//      +--+--+    +--+--+
//      |     |    |     |
//   lvalue   xvalue   prvalue

// An lvalue has an address that your program can access. Examples of lvalue expressions include
// variable names, including const variables, array elements, function calls that return an lvalue
// reference, bit-fields, unions, and class members.
//
// A prvalue expression has no address that is accessible by your program. Examples of prvalue
// expressions include literals, function calls that return a non-reference type, and temporary
// objects that are created during expression evaluation but accessible only by the compiler.
//
// An xvalue expression has an address that no longer accessible by your program but can be used to
// initialize an rvalue reference, which provides access to the expression. Examples include
// function calls that return an rvalue reference, and the array subscript, member and pointer to
// member expressions where the array or object is an rvalue reference.

// Note
// The examples illustrate correct and incorrect usage when operators are not overloaded. By
// overloading operators, you can make an expression such as j * 4 an lvalue.
void test1() {
  int i, j, *p = &j;

  // Correct usage: the variable i is an lvalue and the literal 7 is a prvalue.
  i = 7;

  // Incorrect usage: The left operand must be an lvalue.`j * 4` is a prvalue.
  // ERROR: lvalue required as left operand of assignment
  // 7 = i;
  // ERROR: lvalue required as left operand of assignment
  // j* 4 = 7;

  // Correct usage: the dereferenced pointer is an lvalue.
  *p = i;

  // Correct usage: the conditional operator returns an lvalue.
  ((i < 3) ? i : j) = 7;

  // Incorrect usage: the constant ci is a non-modifiable lvalue.
  [[maybe_unused]] const int ci = 7;
  // ERROR: assignment of read-only variable 'ci'
  // ci = 9;
}

// The following expressions are xvalue expressions:
//
//  * a function call or an overloaded operator expression, whose return type is rvalue reference to
//    object, such as std::move(x);
//  * a[n], the built-in subscript expression, where one operand is an array rvalue;
//  * a.m, the member of object expression, where a is an rvalue and m is a non-static data member
//    of non-reference type;
//  * a.*mp, the pointer to member of object expression, where a is an rvalue and mp is a pointer to
//    data member;
//  * a ? b : c, the ternary conditional expression for certain b and c (see definition for detail);
//  * a cast expression to rvalue reference to object type, such as static_cast<char&&>(x);
//  * any expression that designates a temporary object, after temporary materialization.(since
//    C++17)
// Properties:
//
//  * Same as rvalue (below).
//  * Same as glvalue (below).
// In particular, like all rvalues, xvalues bind to rvalue references, and like all glvalues,
// xvalues may be polymorphic, and non-class xvalues may be cv-qualified.

// Mixed categories
// glvalue
// A glvalue expression is either lvalue or xvalue.
//
// Properties:
//
// * A glvalue may be implicitly converted to a prvalue with lvalue-to-rvalue, array-to-pointer, or
// function-to-pointer implicit conversion.
// * A glvalue may be polymorphic: the dynamic type of the object it identifies is not necessarily
// the static type of the expression.
// * A glvalue can have incomplete type, where permitted by the expression.
//
// rvalue
// An rvalue expression is either prvalue or xvalue.
//
// Properties:
//
// * Address of an rvalue cannot be taken by built-in address-of operator: &int, &i++, &42, and
//   &std::move(x) are invalid. (Assuming i has built-in type or the postincrement operator is not
//   overloaded to return by lvalue reference.)
// * An rvalue can't be used as the left-hand operand of the built-in assignment or compound
//   assignment operators.
// * An rvalue may be used to initialize a const lvalue reference, in which case the lifetime of the
//   object identified by the rvalue is extended until the scope of the reference ends.
// * An rvalue may be used to initialize an rvalue reference, in which case the lifetime of the
//   object identified by the rvalue is extended until the scope of the reference ends.
// * When used as a function argument and when two overloads of the function are available, one
//   taking rvalue reference parameter and the other taking lvalue reference to const parameter, an
//   rvalue binds to the rvalue reference overload (thus, if both copy and move constructors are
//   available, an rvalue argument invokes the move constructor, and likewise with copy and move
//   assignment operators).

int main() {
  test1();
  return 0;
}
