#include <array>
#include <iostream>
#include <vector>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_scan.h>

template<typename BinaryOperation, typename C, typename T> class ParallelScanBody {
public:
  ParallelScanBody(const C& x, C& y, BinaryOperation op, const T& identity)
      : x{x}, y{y}, op{op}, identity{identity}, result{identity} {}
  ParallelScanBody(ParallelScanBody& other, tbb::split)
      : x{other.x}, y{other.y}, op{other.op}, identity{other.identity}, result{identity} {}

  template<typename Range, typename Tag> void operator()(const Range& r, Tag) {
    for (auto i = r.begin(); i < r.end(); i++) {
      result = op(result, x[i]);
      if (Tag::is_final_scan())
        y[i] = result;
    }
  }

  void assign(ParallelScanBody& other) noexcept { result = other.result; }
  void reverse_join(ParallelScanBody& other) noexcept { result += other.result; }
  T get() const { return result; }

private:
  const C& x;
  C& y;
  BinaryOperation op;
  const T& identity;
  T result;
};

template<typename BinaryOperation, typename C, typename T>
auto parallel_scan(const C& x, C& y, BinaryOperation op, const T& identity) ->
    typename std::enable_if_t<
        std::is_same<typename std::iterator_traits<typename C::iterator>::iterator_category,
                     std::random_access_iterator_tag>::value,
        T> {
  ParallelScanBody body{x, y, op, identity};
  tbb::parallel_scan(tbb::blocked_range<size_t>{0, std::min(x.size(), y.size())}, body);
  return body.get();
}

int main() {
  constexpr size_t n = 200'000'000ull;
  std::vector<int> v1(n), v2(n);
  tbb::parallel_for(tbb::blocked_range<int>{0, n}, [&](const auto& r) {
    for (auto i = r.begin(); i != r.end(); i++)
      v1[i] = (i % 2 == 0 ? 1 : -1) * i;
  });

  auto sum = parallel_scan(v1, v2, std::plus<int>{}, 0);
  std::cout << sum << '\n';

  return 0;
}
