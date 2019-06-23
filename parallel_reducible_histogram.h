#pragma once
#include <map>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>

/*! operate f for each element of container and merge into one histogram */
template<typename Key, typename Value, typename C, typename F>
class ParallelReducibleHistogram {
 public:
  ParallelReducibleHistogram(const C& container, F f) : container{container}, f{f} {}
  ParallelReducibleHistogram(const ParallelReducibleHistogram& other, tbb::split)
      : container(other.container), f{other.f} {}

  template<typename R>
  void operator()(const R& r) {
    auto tmp = histogram;
    for (auto i = r.begin(); i != r.end(); ++i)
      ++tmp[f(*i)];
    histogram = tmp;
  }

  void join(const ParallelReducibleHistogram& other) {
    for (const auto& [k, v] : other.histogram)
      histogram[k] += v;
  }
  auto get() const noexcept { return histogram; }

 private:
  const C& container;
  F f;

  std::map<Key, Value> histogram;
};

template<typename Key, typename Value, typename C, typename F>
auto parallel_reduce_histogram(const C& container, F f) -> typename std::enable_if_t<
    std::is_same<typename std::iterator_traits<typename C::iterator>::iterator_category,
                 std::random_access_iterator_tag>::value,
    std::map<Key, Value>> {
  ParallelReducibleHistogram<Key, Value, C, F> hist{container, f};
  tbb::parallel_reduce(
      tbb::blocked_range<typename C::const_iterator>{container.cbegin(), container.cend()}, hist);
  return hist.get();
}

template<typename Key, typename Value, typename C>
auto parallel_reduce_histogram(const C& container) {
  return parallel_reduce_histogram<Key, Value>(container, [](auto x) { return x; });
}
