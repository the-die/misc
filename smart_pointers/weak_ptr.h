#ifndef WEAK_PTR_H_
#define WEAK_PTR_H_

#include <utility>

#include "shared_count.h"

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr {
  template <class U>
  friend class SharedPtr;

 public:
  WeakPtr() noexcept = default;

  WeakPtr(const WeakPtr& other) noexcept : data_(other.data_), count_(other.count_) {
    if (count_) ++count_->weak_count;
  }

  WeakPtr(const SharedPtr<T>& other) noexcept : data_(other.data_), count_(other.count_) {
    if (count_) ++count_->weak_count;
  }

  WeakPtr(WeakPtr&& other) noexcept { swap(other); }

  template <typename U>
  WeakPtr(const WeakPtr<U>& other) noexcept : data_(other.data_), count_(other.count_) {
    if (count_) ++count_->shared_count;
  }

  ~WeakPtr() { Decrement(); }

  WeakPtr& operator=(const WeakPtr& other) noexcept {
    WeakPtr tmp(other);
    swap(tmp);
    return *this;
  }

  WeakPtr& operator=(WeakPtr&& other) noexcept {
    if (this != &other) {
      swap(other);
      other.Reset();
    }
    return *this;
  }

  bool Expired() const noexcept { return !count_ || count_->shared_count == 0; }

  SharedPtr<T> Lock() const { return Expired() ? SharedPtr<T>(nullptr) : SharedPtr<T>(*this); }

  void Reset() noexcept {
    Decrement();
    data_ = nullptr;
    count_ = nullptr;
  }

  void swap(WeakPtr& other) noexcept {
    using std::swap;
    swap(data_, other.data_);
    swap(count_, other.count_);
  }

 private:
  void Decrement() {
    if (!count_) return;

    --count_->weak_count;

    if (count_->shared_count == 0 && count_->weak_count == 0) {
      delete count_;
      count_ = nullptr;
    }
  }

  T* data_ = nullptr;
  SharedCount* count_ = nullptr;
};

#endif  // WEAK_PTR_H_
