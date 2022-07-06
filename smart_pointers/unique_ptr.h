#ifndef UNIQUE_PTR_H_
#define UNIQUE_PTR_H_

#include <cassert>
#include <utility>

template <typename T>
class UniquePtr {
 public:
  UniquePtr() noexcept = default;

  explicit UniquePtr(T* data) : data_(data) {}

  UniquePtr(const UniquePtr& other) = delete;

  UniquePtr(UniquePtr&& other) : data_(other.data_) { other.data_ = nullptr; }

  ~UniquePtr() {
    delete data_;
    data_ = nullptr;
  }

  UniquePtr& operator=(const UniquePtr& other) = delete;

  UniquePtr& operator=(UniquePtr&& other) noexcept {
    swap(other);
    other.Reset();
    return *this;
  }

  T& operator*() const noexcept {
    assert(data_ != nullptr);
    return *data_;
  }

  T* operator->() const noexcept {
    assert(data_ != nullptr);
    return data_;
  }

  explicit operator bool() const noexcept { return data_ != nullptr; }

  T* Get() const noexcept { return data_; }

  T* Release() noexcept {
    T* tmp = data_;
    data_ = nullptr;
    return tmp;
  }

  void Reset(T* data = nullptr) noexcept {
    delete data_;
    data_ = data;
  }

  void swap(UniquePtr& other) noexcept {
    using std::swap;
    swap(data_, other.data_);
  }

 private:
  T* data_ = nullptr;
};

#endif  // UNIQUE_PTR_H_
