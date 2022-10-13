#include <cassert>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>

#include <immintrin.h>

static inline void memcpy_avx_64(void* dst, const void* src) {
  __m256i m0 = _mm256_loadu_si256(((const __m256i*)src) + 0);
  __m256i m1 = _mm256_loadu_si256(((const __m256i*)src) + 1);
  _mm256_storeu_si256(((__m256i*)dst) + 0, m0);
  _mm256_storeu_si256(((__m256i*)dst) + 1, m1);
}

static inline void memcpy_avx512_64(void* dst, const void* src) {
  __m512i m0 = _mm512_loadu_si512(((const __m512i*)src) + 0);
  _mm512_storeu_si512(((__m512i*)dst) + 0, m0);
}

auto gettime() { return std::chrono::steady_clock::now(); }

template <typename T>
auto duration(const T& start, const T& end) {
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

void sleepms(unsigned milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void benchmark(bool dst_align, bool src_align, size_t size, int times) {
  std::srand(std::time(nullptr));
  auto p1 = static_cast<char*>(malloc(size + 64));
  auto p2 = static_cast<char*>(malloc(size + 64));
  auto n1 = reinterpret_cast<size_t>(p1);
  auto n2 = reinterpret_cast<size_t>(p2);
  auto q1 = reinterpret_cast<char*>(((64 - (n1 & 63)) & 63) + n1);
  auto q2 = reinterpret_cast<char*>(((64 - (n2 & 63)) & 63) + n2);
  assert((size_t)q1 % 64 == 0 && (size_t)q2 % 64 == 0);
  auto dst = dst_align ? q1 : q1 + 1;
  auto src = src_align ? q2 : q2 + 3;
  assert(dst <= p1 + 64 && src <= p2 + 64);

  sleepms(100);
  auto t1 = gettime();
  for (int i = 0; i < times; ++i) {
    for (size_t j = 0; j < size; ++j) src[j] = std::rand() % UCHAR_MAX;
    __sync_synchronize();
    memcpy(dst, src, size);
    __sync_synchronize();
  }
  auto v1 = duration(t1, gettime());

  sleepms(100);
  auto t2 = gettime();
  for (int i = 0; i < times; ++i) {
    for (size_t j = 0; j < size; ++j) src[j] = std::rand() % UCHAR_MAX;
    __sync_synchronize();
    memcpy_avx_64(dst, src);
    __sync_synchronize();
  }
  auto v2 = duration(t2, gettime());

  sleepms(100);
  auto t3 = gettime();
  for (int i = 0; i < times; ++i) {
    for (size_t j = 0; j < size; ++j) src[j] = std::rand() % UCHAR_MAX;
    __sync_synchronize();
    memcpy_avx512_64(dst, src);
    __sync_synchronize();
  }
  auto v3 = duration(t3, gettime());

  std::cout << "benchmark: size = " << size << ", times = " << times << std::endl;
  std::cout << "dst aligned: " << std::boolalpha << dst_align << ", src aligned: " << src_align
            << ", memcpy: " << v1 << " us"
            << ", memcpy_avx_64: " << v2 << " us"
            << ", memcpy_avx512_64: " << v3 << " us" << std::endl;

  free(p2);
  free(p1);
}

void test(size_t size, int times) {
  benchmark(true, true, size, times);
  benchmark(true, false, size, times);
  benchmark(false, true, size, times);
  benchmark(false, false, size, times);
}

int main() {
  // only test 64 bytes
  test(64, 0x1000000);
  return 0;
}
