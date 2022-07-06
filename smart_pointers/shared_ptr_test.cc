#include "shared_ptr.h"

void TestSharedPtr() {
  {
    SharedPtr<int> sp1;
    assert(sp1.Get() == nullptr);
    assert(!sp1);
  }

  {
    SharedPtr<int> sp1(new int(1));
    assert(*sp1 == 1);
    assert(*sp1.Get() == 1);
  }

  {
    SharedPtr<int> sp1(new int(1));
    SharedPtr<int> sp2(sp1);
    assert(*sp1 == 1);
    assert(*sp2 == 1);
  }

  {
    SharedPtr<int> sp1(new int(1));
    SharedPtr<int> sp2(std::move(sp1));
    assert(sp1.Get() == nullptr);
    assert(!sp1);
    assert(*sp2 == 1);
  }

  {
    SharedPtr<int> sp1(new int(1));
    SharedPtr<int> sp2(new int(2));
    SharedPtr<int> sp3;

    sp3 = sp1;
    assert(*sp1 == 1);
    assert(*sp3 == 1);

    sp3 = sp2;
    assert(*sp2 == 2);
    assert(*sp3 == 2);
  }

  {
    SharedPtr<int> sp1(new int(1));
    SharedPtr<int> sp2(new int(2));

    SharedPtr<int> sp3;

    sp3 = std::move(sp1);
    assert(sp1.Get() == nullptr);
    assert(!sp1);
    assert(*sp3 == 1);

    sp3 = std::move(sp2);
    assert(sp2.Get() == nullptr);
    assert(!sp2);
    assert(*sp3 == 2);
  }

  {
    SharedPtr<int> sp1(new int(1));
    sp1.Reset(new int(2));
    assert(*sp1 == 2);
    sp1.Reset();
    assert(sp1.Get() == nullptr);
    assert(!sp1);
  }

  {
    class Base {};
    class Derived : public Base {};
    SharedPtr<Derived> sp1(new Derived);
    SharedPtr<Base> sp2(sp1);
  }
}

int main() {
  TestSharedPtr();
  return 0;
}
