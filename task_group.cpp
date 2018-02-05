#include <cstdio>
#include <iostream>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>

unsigned fibonacci(unsigned n) {
  if (n <= 1) {
    return n;
  } else {
    tbb::task_group g;
    unsigned x, y;
    g.run([&] { x = fibonacci(n - 2); });
    g.run([&] { y = fibonacci(n - 1); });
    g.wait();
    return x + y;
  }
}

int main(int argc, char** argv) {
  tbb::task_scheduler_init init{argc == 2 ? std::atoi(argv[1])
                                          : tbb::task_scheduler_init::default_num_threads()};
  std::cout << fibonacci(35) << '\n';

  return 0;
}
