#include "thrdpool.h"

#include <pthread.h>

#include <cstddef>
#include <cstring>

pthread_t Thrdpool::zero_tid_;

static void* ThrdpoolRoutine(void* arg) {
  auto pool = reinterpret_cast<Thrdpool*>(arg);
  pthread_setspecific(pool->key_, pool);
  while (!pool->terminate_) {
    auto entry = reinterpret_cast<ThrdpoolTaskEntry*>(pool->msgqueue_->Get());
    if (!entry) break;

    auto task_routine = entry->task.routine;
    auto task_context = entry->task.context;
    free(entry);
    task_routine(task_context);

    if (pool->nthreads_ == 0) {
      return nullptr;
    }
  }

  pthread_mutex_lock(&pool->mutex_);
  pthread_t tid = pool->tid_;
  pool->tid_ = pthread_self();
  if (--pool->nthreads_ == 0) pthread_cond_signal(pool->terminate_);
  pthread_mutex_unlock(&pool->mutex_);

  if (memcmp(&tid, &Thrdpool::zero_tid_, sizeof(tid)) != 0) pthread_join(tid, NULL);

  return nullptr;
}

bool Thrdpool::InitLocks() {
  if (pthread_mutex_init(&mutex_, NULL) == 0) return true;
  return false;
}

void Thrdpool::DestroyLocks() { pthread_mutex_destroy(&mutex_); }

void Thrdpool::Terminate(bool in_pool) {
  pthread_cond_t term = PTHREAD_COND_INITIALIZER;

  pthread_mutex_lock(&mutex_);
  msgqueue_->SetNonblock();
  terminate_ = &term;

  if (in_pool) {
    pthread_detach(pthread_self());
    --nthreads_;
  }

  while (nthreads_ > 0) pthread_cond_wait(&term, &mutex_);

  pthread_mutex_unlock(&mutex_);

  if (memcmp(&tid_, &zero_tid_, sizeof(tid_)) != 0)
    pthread_join(tid_, NULL);
}

bool Thrdpool::CreateThreads(size_t nthreads) {
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) == 0) {
    if (stacksize_ != 0) pthread_attr_setstacksize(&attr, stacksize_);

    while (nthreads_ < nthreads) {
      int ret = pthread_create(&tid_, &attr, ThrdpoolRoutine, this);
      if (ret == 0)
        ++nthreads_;
      else
        break;
    }

    pthread_attr_destroy(&attr);
    if (nthreads_ == nthreads) return true;

    Terminate(false);
  }
  return false;
}

bool Thrdpool::Create(size_t nthreads, size_t stacksize) {
  if (InitLocks()) {
    if (pthread_key_create(&key_, NULL) == 0) {
      msgqueue_ = new MsgQueue(static_cast<size_t>(-1), 0);
      stacksize_ = stacksize;
      memset(&tid_, 0, sizeof(tid_));
      if (CreateThreads(nthreads)) return true;
      delete msgqueue_;
      pthread_key_delete(key_);
    }
    DestroyLocks();
  }
  return false;
}

bool Thrdpool::Schedule(const ThrdpoolTask& task) {
  auto entry = new ThrdpoolTaskEntry;
  entry->task = task;
  msgqueue_->Put(entry);
  return true;
}

bool Thrdpool::Increase() {
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) == 0) {
    if (stacksize_) pthread_attr_setstacksize(&attr, stacksize_);

    pthread_mutex_lock(&mutex_);
    pthread_t tid;
    int ret = pthread_create(&tid, &attr, ThrdpoolRoutine, this);
    if (ret == 0) ++nthreads_;

    pthread_mutex_unlock(&mutex_);
    pthread_attr_destroy(&attr);
    if (ret == 0) return true;
  }
  return false;
}

bool Thrdpool::InPool() { return pthread_getspecific(key_) == this; }

void Thrdpool::Destroy(void (*pending)(const ThrdpoolTask&)) {
  bool in_pool = InPool();
  Terminate(in_pool);
  while (true) {
    auto entry = reinterpret_cast<ThrdpoolTaskEntry*>(msgqueue_->Get());
    if (!entry) break;

    if (pending) pending(entry->task);

    free(entry);
  }

  pthread_key_delete(key_);
  DestroyLocks();
}
