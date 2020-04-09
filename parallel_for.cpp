#include <iostream>
#include <thread>
#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/task_group.h>
#include <tbb/tick_count.h>

void blocked_range(size_t n = 100, size_t grain_size = 1) {
  const auto start = tbb::tick_count::now();
  constexpr auto func = __func__;
  tbb::parallel_for(tbb::blocked_range<size_t>{0, n, grain_size}, [&](const auto& r) {
    for (auto i = r.begin(); i != r.end(); i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds{(n - i) * 10});
      std::cout << func << '\t' << i << '\n';
    }
  });
  std::clog << func << '\t' << (tbb::tick_count::now() - start).seconds() << '\n';
}

void blocked_range2d(size_t n = 10, size_t row_grainsize = 1, size_t col_grainsize = 1) {
  const auto start = tbb::tick_count::now();
  constexpr auto func = __func__;
  tbb::parallel_for(tbb::blocked_range2d<size_t>{0, n, row_grainsize, 0, n, col_grainsize},
                    [&](const auto& r) {
                      const auto rows = r.rows();
                      const auto cols = r.cols();
                      for (auto i = rows.begin(), ie = rows.end(); i != ie; i++)
                        for (auto j = cols.begin(), je = cols.end(); j != je; j++) {
                          std::this_thread::sleep_for(std::chrono::milliseconds{(i + j) * 10});
                          std::cout << func << '\t' << i << ',' << j << '\n';
                        }
                    });
  std::clog << func << '\t' << (tbb::tick_count::now() - start).seconds() << '\n';
}

void blocked_range3d(size_t n = 5,
                     size_t page_grainsize = 1,
                     size_t row_grainsize = 1,
                     size_t col_grainsize = 1) {
  const auto start = tbb::tick_count::now();
  constexpr auto func = __func__;
  tbb::parallel_for(
      tbb::blocked_range3d<size_t>{0, n, page_grainsize, 0, n, row_grainsize, 0, n, col_grainsize},
      [&](const auto& r) {
        const auto pages = r.pages();
        const auto rows = r.rows();
        const auto cols = r.cols();
        for (auto i = pages.begin(), ie = pages.end(); i != ie; i++)
          for (auto j = rows.begin(), je = rows.end(); j != je; j++)
            for (auto k = cols.begin(), ke = cols.end(); k != ke; k++) {
              std::this_thread::sleep_for(std::chrono::milliseconds{(i + j + k) * 10});
              std::cout << func << '\t' << i << ',' << j << ',' << k << '\n';
            }
      });
  std::clog << func << '\t' << (tbb::tick_count::now() - start).seconds() << '\n';
}

int main() {
  tbb::task_arena a{64};
  tbb::task_group g;
  a.execute([&] { g.run([] { blocked_range(); }); });
  a.execute([&] { g.run([] { blocked_range2d(); }); });
  a.execute([&] { g.run([] { blocked_range3d(); }); });
  a.execute([&] { g.wait(); });

  return 0;
}
