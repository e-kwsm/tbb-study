#include <array>
#include <iostream>
#include <type_traits>
#include <tbb/pipeline.h>

template<typename T>
auto norm_sq(const T* begin, const T* const end, size_t max_number_of_live_tokens)
    -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
  T sum = 0;
  tbb::parallel_pipeline(
      max_number_of_live_tokens,
      tbb::make_filter<void, const T*>(tbb::filter::serial, [&](auto& fc) -> const T* {
        if (begin < end) {
          return begin++;
        } else {
          fc.stop();
          return nullptr;
        }
      }) & tbb::make_filter<const T*, T>(tbb::filter::parallel, [](auto p) {
        return (*p) * (*p);
      }) & tbb::make_filter<T, void>(tbb::filter::serial, [&](auto x) { sum += x; }));
  return sum;
}

template<typename C>
auto norm_sq(const C& c, size_t max_number_of_live_tokens) {
  return norm_sq(std::data(c), std::data(c) + std::size(c), max_number_of_live_tokens);
}

int main() {
  constexpr size_t n = 1'000'000ull;
  std::array<double, n> a;
  a.fill(1.0);
  std::cout << norm_sq(a, 8) << '\n';
}
