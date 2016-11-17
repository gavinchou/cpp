#include <iostream>
#include <functional>

class Defer {
  std::function<void()> func;
public:
  Defer(std::function<void()> f) {
    func = f;
  }
  ~Defer() {
    try {
      func();
    } catch (...) {
      // do nothing
    }
  }
};

void test_defer() {
  Defer d([]()->void {
      std::cout << "this will be deferred" << std::endl;
    });
  std::cout << "this runs before deferred" << std::endl;
}

int main_(void) {
  test_defer();
  return 0;
}

// vim: et ts=2 sw=2 tw=80:
