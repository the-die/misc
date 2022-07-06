#include "unique_ptr.h"

void TestUniquePtr() {
  {
    UniquePtr<int> up1(new int(1));
    assert(*up1 == 1);
    assert(*up1.Get() == 1);
  }

  {
    UniquePtr<int> up1(new int(1));
    UniquePtr<int> up2(std::move(up1));
  }

  {
    UniquePtr<int> up1(new int(1));
    UniquePtr<int> up2;
    up2 = std::move(up1);
  }

  {
    UniquePtr<int> up1(new int(1));
    up1 = std::move(up1);
  }

  {
    UniquePtr<int> up1(new int(1));
    up1.Reset(new int(2));
    assert(*up1 == 2);
    up1.Reset();
  }

  {
    UniquePtr<int> up1(new int(1));
    int* data = up1.Release();
    assert(!up1);
    delete data;
  }
}

int main() {
  TestUniquePtr();
  return 0;
}
