#include <array>
#include <cassert>
#include <iostream>
#include <thread>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

int main() {
  constexpr size_t n = 100;
  std::array<int, n> a;
  tbb::parallel_for(tbb::blocked_range<size_t>{0, n}, [&](const auto& r) {
    for (auto i = r.begin(); i != r.end(); i++) {
      a[i] = i;
      std::this_thread::sleep_for(std::chrono::milliseconds{(n - i) * 10});
      std::cout << '#' << i << '\n';
    }
  });
  for (size_t i = 0; i < n; i++)
    assert(a[i] == i);

  return 0;
}
