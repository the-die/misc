#ifndef COMMON_H_
#define COMMON_H_

#include <cassert>
#include <cstddef>

constexpr size_t kMaxObjectSize = 64 * 1024;
constexpr size_t kMaxListSize = 184;
constexpr size_t kPageShift = 12;
constexpr size_t kMaxPageNum = 129;

struct Object {
  Object* next;
};

class FreeList {
 public:
  void Push(void* ptr) noexcept {
    auto node = reinterpret_cast<Object*>(ptr);
    node->next = list_;
    list_ = node;
    ++size_;
  }

  void PushRange(void* start, void* end, size_t n) noexcept {
    auto tmp = reinterpret_cast<Object*>(end);
    tmp->next = list_;
    list_ = reinterpret_cast<Object*>(start);
    size_ += n;
  }

  void* Pop() noexcept {
    if (Empty()) return nullptr;
    void* ret = list_;
    list_ = list_->next;
    --size_;
    return ret;
  }

  void* PopRange() noexcept {
    size_ = 0;
    void* ret = list_;
    list_ = nullptr;
    return ret;
  }

  bool Empty() const noexcept { return list_ == nullptr; }

  size_t Size() const noexcept { return size_; }

  size_t MaxSize() const noexcept { return max_size_; }

  void SetMaxSize(size_t max_size) noexcept { max_size_ = max_size; }

 private:
  Object* list_ = nullptr;
  size_t size_ = 0;
  size_t max_size_ = 1;
};

inline static size_t Index(size_t size, size_t align) {
  size_t align_num = 0x1 << align;
  return ((size + align_num - 1) >> align) - 1;
}

inline static size_t RoundUp(size_t size, size_t align) {
  size_t align_num = 0x1 << align;
  return (size + align_num - 1) & ~(align_num - 1);
}

// [1, 128]                 8-byte alignment freelist[0, 16)
// [129, 1024]             16-byte alignment freelist[16, 72)
// [1025, 8 * 1024]       128-byte alignment freelist[72, 128)
// [8 * 1024, 64 * 1024] 1024-byte alignment freelist[128, 184)
inline size_t Index(size_t size) {
  assert(size < kMaxObjectSize);

  constexpr size_t kGroupSize[] = {16, 56, 56, 56};
  if (size <= 128)
    return Index(size, 3);
  else if (size <= 1024)
    return Index(size - 128, 4) + kGroupSize[0];
  else if (size <= 8192)
    return Index(size - 1024, 7) + kGroupSize[0] + kGroupSize[1];
  else
    return Index(size - 8 * 1024, 10) + kGroupSize[0] + kGroupSize[1] + kGroupSize[2];
}

inline size_t RoundUp(size_t size) {
  assert(size < kMaxObjectSize);

  if (size <= 128)
    return RoundUp(size, 3);
  else if (size <= 1024)
    return RoundUp(size, 4);
  else if (size <= 8192)
    return RoundUp(size, 7);
  else
    return RoundUp(size, 10);
}

inline size_t NumMoveSize(size_t size) {
  if (size == 0) return 0;

  size_t num = kMaxObjectSize / size;
  if (num < 2) num = 2;

  if (num > 512) num = 512;

  return num;
}

#endif  // COMMON_H_
