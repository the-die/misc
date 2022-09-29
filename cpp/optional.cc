#include <functional>
#include <ios>
#include <iostream>
#include <optional>
#include <string>

// optional can be used as the return type of a factory that may fail
std::optional<std::string> create(bool b) {
  if (b) return "Godzilla";
  return {};
}

// std::nullopt can be used to create any (empty) std::optional
// std::nullopt is a constant of type std::nullopt_t that is used to indicate optional type with
// uninitialized state.
auto create2(bool b) { return b ? std::optional<std::string>{"Godzilla"} : std::nullopt; }

// std::reference_wrapper may be used to return a reference
auto create_ref(bool b) {
  static std::string value = "Godzilla";
  return b ? std::optional<std::reference_wrapper<std::string>>{value} : std::nullopt;
}

int main() {
  std::optional<int> o1;                        // empty
  std::optional o2 = 1;                         // init from rvalue
  [[maybe_unused]] std::optional<int> o3 = o2;  // copy-constructor

  // Move-constructed from std::string using deduction guide to pick the type
  std::optional o4(std::string{"deduction"});

  std::cout << "create(false) returned " << create(false).value_or("empty") << std::endl;

  // optional-returning factory functions are usable as conditions of while and if
  if (auto str = create2(true)) {
    std::cout << "create2(true) returned " << *str << std::endl;
  }

  if (auto str = create_ref(true)) {
    // using get() to access the reference_wrapper's value
    std::cout << "create_ref(true) returned " << str->get() << std::endl;
    str->get() = "Mothra";
    std::cout << "modifying it changed it to " << str->get() << std::endl;
  }

  // Accesses the contained value.
  // operator->: Returns a pointer to the contained value.
  // operator*: Returns a reference to the contained value.
  // The behavior is undefined if *this does not contain a value.

  std::optional<int> opt1 = 1;
  std::cout << "opt1: " << *opt1 << std::endl;

  *opt1 = 2;
  std::cout << "opt1: " << *opt1 << std::endl;

  std::optional<std::string> opt2 = "abc";
  std::cout << "opt2: " << *opt2 << " size: " << opt2->size() << std::endl;

  // You can "take" the contained value by calling operator* on a rvalue to optional
  auto taken = *std::move(opt2);
  std::cout << "taken: " << taken << " opt2: " << *opt2 << "size: " << opt2->size() << std::endl;

  // operator bool
  // has_value
  // Checks whether *this contains a value.

  std::cout << std::boolalpha;

  std::optional<int> opt;
  std::cout << opt.has_value() << std::endl;

  opt = 43;
  if (opt)
    std::cout << "value set to " << opt.value() << std::endl;
  else
    std::cout << "value not set" << std::endl;

  opt.reset();
  if (opt.has_value())
    std::cout << "value still set to " << opt.value() << std::endl;
  else
    std::cout << "value no longer set" << std::endl;
}
