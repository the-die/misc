#ifndef BIG_UINI_H_
#define BIG_UINI_H_

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <vector>

// UINT64_MAX: 18446744073709551615, size 20
template <typename T = std::uint64_t, T m = 1000000000ULL, unsigned places = 9>
class BigUint {
  friend std::ostream& operator<<(std::ostream& os, const BigUint& n) {
    auto it = n.data_.rbegin();
    os << *it++;
    while (it != n.data_.rend()) {
      os << std::setfill('0') << std::setw(places) << *it++;
    }
    return os;
  }

 public:
  BigUint() = default;

  BigUint(T x) {
    assert(x < m);
    data_.push_back(x);
  }

  BigUint operator+(const BigUint& right) const {
    BigUint sum;
    T carry = 0;
    auto& other = right.data_;
    auto n = std::max(data_.size(), other.size());
    sum.data_.reserve(n + 1);
    for (auto i = 0; i < n; ++i) {
      T a = i < data_.size() ? data_[i] : 0;
      T b = i < other.size() ? other[i] : 0;
      sum.data_.push_back((a + b + carry) % m);
      carry = (a + b + carry) / m;
    }
    if (carry > 0) sum.data_.push_back(carry);
    return sum;
  }

  BigUint operator*(const BigUint& right) const {
    BigUint product;
    auto& other = right.data_;
    product.data_.resize(data_.size() + other.size());
    for (auto i = 0; i < other.size(); ++i) {
      for (auto j = 0; j < data_.size(); ++j) {
        T lo = (other[i] * data_[j] + product.data_[i + j]) % m;
        T hi = (other[i] * data_[j] + product.data_[i + j]) / m;
        product.data_[i + j] = lo;
        product.data_[i + j + 1] += hi;
      }
    }
    while (product.data_.size() > 1 && product.data_.back() == 0) product.data_.pop_back();
    return product;
  }

 private:
  std::vector<T> data_;
};

#endif  // BIG_UINI_H_
