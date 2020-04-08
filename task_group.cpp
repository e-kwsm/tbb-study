#include <iostream>
#include <tbb/task_group.h>

unsigned fibonacci(unsigned n) {
  if (n <= 1u)
    return n;
  tbb::task_group g;
  unsigned x, y;
  g.run([&] { x = fibonacci(n - 2u); });
  y = fibonacci(n - 1u);
  g.wait();
  return x + y;
}

int main() {
  std::cout << fibonacci(35) << '\n';  // 9227465

  return 0;
}
