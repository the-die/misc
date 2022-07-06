#ifndef SHARED_COUNT_H_
#define SHARED_COUNT_H_

#include <cstddef>

struct SharedCount {
  size_t shared_count = 1;
  size_t weak_count = 0;
};

#endif  // SHARED_COUNT_H_
