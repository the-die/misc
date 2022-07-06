#include "shared_ptr.h"
#include "weak_ptr.h"

void TestWeakPtr() {
  {
    WeakPtr<int> wp1;
    assert(!wp1.Lock());
    assert(wp1.Expired());
  }

  {
    SharedPtr<int> sp1(new int(1));
    WeakPtr<int> wp1(sp1);
    assert(wp1.Lock());
    wp1.Reset();
    assert(wp1.Expired());
  }

  {
    SharedPtr<int> sp1(new int(1));
    WeakPtr<int> wp1(sp1);
    WeakPtr<int> wp2(wp1);
    assert(wp1.Lock());
    assert(wp2.Lock());
  }

  {
    SharedPtr<int> sp1(new int(1));
    WeakPtr<int> wp1(sp1);
    WeakPtr<int> wp2(std::move(wp1));
    assert(wp1.Expired());
    assert(wp2.Lock());
  }

  {
    SharedPtr<int> sp1(new int(1));
    SharedPtr<int> sp2(new int(2));
    WeakPtr<int> wp1(sp1);
    WeakPtr<int> wp2(sp2);
    WeakPtr<int> wp3;
    wp3 = wp1;
    assert(wp1.Lock());
    assert(wp3.Lock());
    wp3 = wp2;
    assert(wp2.Lock());
    assert(wp3.Lock());
  }

  {
    SharedPtr<int> sp1(new int(1));
    SharedPtr<int> sp2(new int(1));
    WeakPtr<int> wp1(sp1);
    WeakPtr<int> wp2(sp1);
    WeakPtr<int> wp3;
    wp3 = std::move(wp1);
    assert(wp1.Expired());
    assert(wp3.Lock());
    wp3 = std::move(wp2);
    assert(wp2.Expired());
    assert(wp3.Lock());
  }

  {
    SharedPtr<int> sp1(new int(1));
    WeakPtr<int> wp(sp1);
    {
      auto sp2 = wp.Lock();
      assert(sp2);
    }
    sp1.Reset();
    assert(!wp.Lock());
    assert(wp.Expired());
  }
}

int main() {
  TestWeakPtr();
  return 0;
}
