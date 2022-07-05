#ifndef MSGQUEUE_H_
#define MSGQUEUE_H_

#include <condition_variable>
#include <cstddef>
#include <mutex>

class MsgQueue {
 public:
  MsgQueue(size_t maxlen, ptrdiff_t linkoff) : msg_max_(maxlen), linkoff_(linkoff) {
    get_head_ = &head1_;
    put_head_ = &head2_;
    put_tail_ = &head2_;
  }

  ~MsgQueue() {}

  void Put(void* msg) {
    auto link = reinterpret_cast<void**>(reinterpret_cast<char*>(msg) + linkoff_);
    *link = nullptr;

    // lock producer
    std::unique_lock<std::mutex> put_lock(put_mutex_);

    while (msg_cnt_ >= msg_max_ && !nonblock_) {
      put_cond_.wait(put_lock);
    }

    *put_tail_ = link;
    put_tail_ = link;
    ++msg_cnt_;

    // unlock producer
    put_lock.unlock();
    // unblock one of consumer
    get_cond_.notify_one();
  }

  void* Get() {
    void* msg;

    // lock consumer
    std::unique_lock<std::mutex> get_lock(get_mutex_);

    if (*get_head_ || MsgQueueSwap() > 0) {
      msg = reinterpret_cast<char*>(*get_head_) - linkoff_;
      *get_head_ = *reinterpret_cast<void**>(*get_head_);
    } else {
      msg = nullptr;
    }

    // unlock consumer and return
    return msg;
  }

  void SetNonblock() {
    nonblock_ = true;
    std::lock_guard<std::mutex> put_lock(put_mutex_);
    // unlock one consumer
    get_cond_.notify_one();
    // unlock all producers
    put_cond_.notify_all();
  }

  void SetBlock() { nonblock_ = false; }

 private:
  // consumer has been locked
  size_t MsgQueueSwap() {
    void** get_head = get_head_;
    get_head_ = put_head_;

    // lock producer
    std::unique_lock<std::mutex> put_lock(put_mutex_);

    while (msg_cnt_ == 0 && !nonblock_) {
      get_cond_.wait(put_lock);
    }

    size_t cnt = msg_cnt_;
    // If the current producer queue is full, it means there may by more than one producer waiting.
    if (cnt >= msg_max_) put_cond_.notify_all();

    put_head_ = get_head;
    put_tail_ = get_head;
    msg_cnt_ = 0;

    // unlock producer and return
    return cnt;
  }

  size_t msg_max_;
  size_t msg_cnt_ = 0;
  ptrdiff_t linkoff_;
  bool nonblock_ = false;

  // helper nodes
  void* head1_ = nullptr;
  void* head2_ = nullptr;

  void** get_head_;
  void** put_head_;
  void** put_tail_;

  std::mutex get_mutex_;
  std::mutex put_mutex_;
  std::condition_variable get_cond_;
  std::condition_variable put_cond_;
};

#endif  // MSGQUEUE_H_
