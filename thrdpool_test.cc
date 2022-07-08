#include "thrdpool.h"

void Routine(void* context) {
  printf("task-%llu start.\n", reinterpret_cast<unsigned long long>(context));
}

void Pending(const ThrdpoolTask& task) {
  printf("pending task-%llu.\n", reinterpret_cast<unsigned long long>(task.context));
}

int main() {
  Thrdpool thrd_pool;
  thrd_pool.Create(3, 1024);
  ThrdpoolTask task;
  unsigned long long i;

  for (i = 0; i < 500000; i++) {
    task.routine = &Routine;
    task.context = reinterpret_cast<void*>(i);
    thrd_pool.Schedule(task);
  }
  getchar();
  thrd_pool.Destroy(&Pending);
  return 0;
}
