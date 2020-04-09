#include <array>
#include <iostream>
#include <tbb/blocked_range.h>
#include "parallel_reducible_histogram.h"

int main() {
  constexpr size_t n = 1'000'000;
  using array_t = std::array<int, n>;
  array_t a;
  for (size_t i = 0; i < n; i++)
    a[i] = i;

  for (auto [k, v] : parallel_reduce_histogram<size_t, unsigned>(a, [](auto k) {
         constexpr auto bin = n / 10;
         return (k / bin) * bin;
       }))
    std::cout << k << '\t' << v << '\n';

  return 0;
}
