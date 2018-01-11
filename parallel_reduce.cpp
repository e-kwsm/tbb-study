#include <iostream>
#include <type_traits>
#include <vector>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>

template<typename T>
auto sum_parallel_reduce(const T& container) -> typename std::enable_if_t<
    std::is_same<typename std::iterator_traits<typename T::iterator>::iterator_category,
                 std::random_access_iterator_tag>::value,
    typename T::value_type> {
  using result_type = typename T::value_type;
  result_type sum = tbb::parallel_reduce(
      tbb::blocked_range<size_t>{0, container.size()}, static_cast<result_type>(0),
      [&](const auto& range, result_type value) -> result_type {
        for (auto i = range.begin(); i != range.end(); i++) {
          value += container[i];
        }
        return value;
      },
      std::plus<result_type>{});
  return sum;
}

int main() {
  constexpr size_t n = 500'000'000ull;
  std::vector<int> v(n, 1);
  std::cout << sum_parallel_reduce(v) << std::endl;

  return 0;
}
