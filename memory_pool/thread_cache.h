#ifndef THREAD_CACHE_H_
#define THREAD_CACHE_H_

#include <algorithm>

#include "common.h"

class ThreadCache {
 public:
  void* Allocate(size_t size) {
    size_t idx = Index(size);

    auto& freelist = freelist_[idx];
    if (freelist.Empty())
      return FetchFromCentralCache(idx, RoundUp(size));
    else
      return freelist.Pop();
  }

  void Deallocate(void* ptr, size_t size) {
    size_t idx = Index(size);
    auto& freelist = freelist_[idx];
    freelist.Push(ptr);
    if (freelist.Size() > freelist.MaxSize()) ListTooLong(freelist, size);
  }

  // TODO
  void* FetchFromCentralCache(size_t idx, size_t size) {
    auto& freelist = freelist_[idx];

    size_t max_size = freelist.MaxSize();
    size_t num_to_move = std::min(NumMoveSize(size), max_size);

    void* start = nullptr;
    void* end = nullptr;
    size_t batchsize = CentralCache::Instance()->FetchRangeObj(start, end, num_to_move, size);

    if (batchsize > 1)
      freelist.PushRange(reinterpret_cast<Object*>(start)->next, end, batchsize - 1);

    if (batchsize >= freelist.MaxSize()) freelist.SetMaxSize(max_size + 1);

    return start;
  }

  // TODO
  void ListTooLong(FreeList& freelist, size_t size) {
    void* start = freelist.PopRange();
    CentralCache::Instance()->ReleaseListToSpans(start, size);
  }

 private:
  FreeList freelist_[kMaxListSize];
};

thread_local ThreadCache* kThreadCache = nullptr;

#endif  // THREAD_CACHE_H_
