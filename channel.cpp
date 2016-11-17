#include <iostream>
#include <future>
#include <thread>

#include "defer.cpp"

template<typename T>
class Channel {
  std::promise<T> pro;
public:
  Channel() : pro() {
  }
  ~Channel() {
  }
  int operator<<(const T& val) {
    pro.set_value(val);
    return 0;
  }
  int operator>>(T& other) {
    other = pro.get_future().get();
    return 0;
  }
};

void test_channel() {
  Channel<int> c;
  std::thread th([&c]()->void {
      int val;
      std::cout <<
        R"(this will be blocked untill "c <<" called)" << std::endl;
      c >> val;
      std::cout << "channel " << val << std::endl;
    });
  Defer d([&th]{ th.join(); });
  std::cout << "Xxxxxxx " << std::endl;
//   c << 10086;
  std::cout << "do the fxxking stuff " << std::endl;
}

int main(void) {
  test_channel();
  return 0;
}

// vim: et ts=2 sw=2 tw=80:
