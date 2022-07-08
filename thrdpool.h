#ifndef THRDPOOL_H_
#define THRDPOOL_H_

#include <pthread.h>

#include <cstddef>

#include "msgqueue.h"

struct ThrdpoolTask {
  void (*routine)(void*);
  void* context;
};

struct ThrdpoolTaskEntry {
  void* link;
  ThrdpoolTask task;
};

static void* ThrdpoolRoutine(void* arg);

class Thrdpool {
  friend void* ThrdpoolRoutine(void* arg);

 public:
  Thrdpool() = default;

  ~Thrdpool() = default;

  bool Create(size_t nthreads, size_t stacksize);

  bool Schedule(const ThrdpoolTask& task);

  bool Increase();

  bool InPool();

  void Destroy(void (*pending)(const ThrdpoolTask&));

 private:
  bool InitLocks();

  void DestroyLocks();

  void Terminate(bool in_pool);

  bool CreateThreads(size_t nthreads);

  static pthread_t zero_tid_;

  MsgQueue* msgqueue_ = nullptr;
  size_t nthreads_ = 0;
  size_t stacksize_ = 0;
  pthread_t tid_;
  pthread_mutex_t mutex_;
  pthread_key_t key_;
  pthread_cond_t* terminate_ = nullptr;
};

#endif  // THRDPOOL_H_
