#ifndef SHARED_PTR_H_
#define SHARED_PTR_H_

#include <cassert>
#include <utility>

#include "shared_count.h"

template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
  template <typename U>
  friend class SharedPtr;

  template <typename U>
  friend class WeakPtr;

 public:
  SharedPtr() noexcept = default;

  // SharedPtr<int> sp(nullptr);
  // SharedPtr<int> sp(new int(2022));
  explicit SharedPtr(T* data) : data_(data) {
    if (data_) {
      count_ = new SharedCount;
    }
  }

  SharedPtr(const SharedPtr& other) noexcept : data_(other.data_), count_(other.count_) {
    if (count_) ++count_->shared_count;
  }

  SharedPtr(SharedPtr&& other) { swap(other); }

  template <typename U>
  SharedPtr(const SharedPtr<U>& other) noexcept : data_(other.data_), count_(other.count_) {
    if (count_) ++count_->shared_count;
  }

  explicit SharedPtr(const WeakPtr<T>& other) noexcept : data_(other.data_), count_(other.count_) {
    if (count_) ++count_->shared_count;
  }

  ~SharedPtr() { Decrement(); }

  // SharedPtr<int> p(new int(2022));
  // p = p;
  // OK
  SharedPtr& operator=(const SharedPtr& other) noexcept {
    SharedPtr tmp(other);
    swap(tmp);
    return *this;
  }

  // SharedPtr<int> p(new int(2022));
  // p = std::move(p);
  // OK
  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (this != &other) {
      swap(other);
      other.Reset();
    }
    return *this;
  }

  T& operator*() const noexcept {
    assert(data_ != nullptr);
    return *data_;
  }

  T& operator->() const noexcept {
    assert(data_ != nullptr);
    return data_;
  }

  explicit operator bool() const noexcept { return data_ != nullptr; }

  T* Get() const noexcept { return data_; }

  void Reset() noexcept {
    Decrement();
    data_ = nullptr;
    count_ = nullptr;
  }

  void Reset(T* data) {
    Decrement();
    data_ = data;
    count_ = new SharedCount;
  }

  void swap(SharedPtr& other) noexcept {
    using std::swap;
    swap(data_, other.data_);
    swap(count_, other.count_);
  }

 private:
  void Decrement() noexcept {
    if (!count_) return;

    --count_->shared_count;

    if (count_->shared_count == 0) {
      delete data_;
      data_ = nullptr;
      if (count_->weak_count == 0) {
        delete count_;
        count_ = nullptr;
      }
    }
  }

  T* data_ = nullptr;
  SharedCount* count_ = nullptr;
};

#endif  // SHARED_PTR_H_
