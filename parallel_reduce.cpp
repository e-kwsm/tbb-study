#include <iostream>
#include <limits>
#include <type_traits>
#include <vector>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>

template<typename BinaryOperation, typename C, typename T>
auto parallel_reduce(const C& container, BinaryOperation op, const T& identity) -> std::enable_if_t<
    std::is_same_v<typename std::iterator_traits<typename C::iterator>::iterator_category,
                   std::random_access_iterator_tag>,
    T> {
  return tbb::parallel_reduce(
      tbb::blocked_range<typename C::const_iterator>{container.cbegin(), container.cend()},
      identity,
      [&](const auto& range, T value) -> T {
        for (auto i = range.begin(); i != range.end(); ++i) {
          value = op(value, *i);
        }
        return value;
      },
      op);
}

int main() {
  constexpr size_t n = 500'000'000ull;
  std::vector<int> v(n, 1);
  std::cout << "sum\t" << parallel_reduce(v, std::plus<int>{}, 0) << std::endl;
  std::cout << "min\t"
            << parallel_reduce(
                   v, [](int a, int b) { return std::min(a, b); }, std::numeric_limits<int>::max())
            << '\n';

  return 0;
}
