#include <iostream>
#include <boost/format.hpp>
#include <tbb/task.h>
#include <tbb/task_scheduler_init.h>

class FibonacciTask : public tbb::task {
 public:
  FibonacciTask(unsigned n, unsigned& sum) : n{n}, sum{sum} {}
  tbb::task* execute() override final {
    if (n < 2u) {
      sum = n;
    } else {
      unsigned x, y;
      auto& a = *new (allocate_child()) FibonacciTask{n - 2u, x};
      auto& b = *new (allocate_child()) FibonacciTask{n - 1u, y};
      set_ref_count(3);  // two children plus one for the wait
      spawn(a);
      spawn_and_wait_for_all(b);
      sum = x + y;
    }
    return nullptr;
  }

 private:
  const unsigned n;
  unsigned& sum;
};

unsigned fibonacci(unsigned n) {
  unsigned sum;
  auto& a = *new (tbb::task::allocate_root()) FibonacciTask{n, sum};
  tbb::task::spawn_root_and_wait(a);
  return sum;
}

int main(int argc, char** argv) {
  tbb::task_scheduler_init init{argc == 2 ? std::atoi(argv[1])
                                          : tbb::task_scheduler_init::default_num_threads()};

  std::cout << fibonacci(35) << '\n';  // 9227465
  return 0;
}
