#include <iostream>
#include <random>

#include "simple_thread_pool.hpp"

int main(void) {
  ThreadPool<> th_pool(10);
  th_pool.start();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(100, 200);
  for (int i = 0; i < 100; ++i) {
    auto random_sleep = dis(gen);
    th_pool.submit([i, random_sleep]{
        std::this_thread::sleep_for(std::chrono::milliseconds(random_sleep));
        std::cout << __PRETTY_FUNCTION__ << "==== " << i
          << " sleep " << random_sleep << std::endl;
      });
  }
//   th_pool.await();
  return 0;
}
// vim: et tw=80 ts=2 sw=2 cc=80:
