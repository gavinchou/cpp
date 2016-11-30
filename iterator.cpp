#include <iostream>
#include <fstream>

#include "iterator.hpp"

struct BucketInfo {
  int64_t id;
  std::string name;
  friend std::ostream& operator<<(std::ostream& os, const BucketInfo& bi) {
    os << "id: " << bi.id << ", name: " << bi.name << std::endl;
    return os;
  }
};

/**
 * An implement of double buffered bucket iterator by reading bucket info from file
 */
class BucketIterator: public DoubleBufferedIterator<BucketInfo> {
private:
  std::fstream _f;

public:
  BucketIterator(size_t buf_size): DoubleBufferedIterator<BucketInfo>(buf_size) {
    init();
  }

  int init() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    _f.open("/users/gavin/tmp/bucket_info.txt");
    if (!_f.is_open()) {
      std::cout << "failed to open file" << std::endl;
      return -1;
    }
    int ret = this->DoubleBufferedIterator::init();
    if (ret != 0) {
      // failed to init iterator
      std::cout << "supper init failed" << std::endl;
      return ret;
    }
    return 0;
  }

  virtual void do_refill_backup() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::string line;
    size_t line_read = 0;
    std::cout << "is open: " << _f.is_open() << std::endl;
    while (line_read < _buf_size && getline(_f, line)) {
      std::cout << "+++++++++ id:" << std::atoi(line.substr(0, line.find(" ")).c_str());
      std::cout << " name:" << line.substr(line.find(" ") + 1, line.length()) << std::endl;
      std::shared_ptr<BucketInfo> bi(new BucketInfo());
      bi->id = std::atoi(line.substr(0, line.find(" ")).c_str());
      bi->name = line.substr(line.find(" ") + 1, line.length());
      _backup_buf->push_back(std::move(bi));
      ++line_read;
      std::this_thread::sleep_for(std::chrono::milliseconds(110));
    }
    if (line_read == 0) {
      std::cout << "all lines read" << std::endl;
    }
  }

  ~BucketIterator() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (_f.is_open()) {
      _f.close();
    }
  }

};

void test_itertor() {
  BucketIterator bi(10);
  int i = 0;
  while (bi.has_next()) {
//   while (true) {
//     std::cout << "has next: " << bi.has_next() << std::endl;
//     std::cout << std::move(bi.next()) << std::endl;
    std::cout << *bi.next() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ++i;
  }
  std::cout << "total elements: " << i << std::endl;
}

int main(void) {
  test_itertor();
  return 0;
}

// vim: et tw=100 ts=2 sw=2 cc=100:
