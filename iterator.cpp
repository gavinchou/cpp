#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <deque>
#include <thread>
#include <future>

template <typename E>
class Iterator {
public:
  virtual bool has_next() = 0;
  virtual int remove() = 0;
  virtual E* next() = 0;
};

struct BucketInfo {
  int64_t id;
  std::string name;
  friend std::ostream& operator<<(std::ostream& os, const BucketInfo& bi) {
    os << "id: " << bi.id << ", name: " << bi.name << std::endl;
    return os;
  }
};

class BucketIterator: public Iterator<BucketInfo> {
public:
  virtual bool has_next() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (current_it != current_buf->end()) {
      std::cout << "current buffer is not empty" << std::endl;
      return true;
    }
    while (_is_refilling) {
      // TODO: cond_wait should be invoked here to improve the performance
      std::cout << "refilling, sleep for a while" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (current_it == current_buf->end()
        && backup_buf->size() > 0) {
      std::cout << "swap buffers" << std::endl;
      current_buf->clear();
      std::swap(current_buf, backup_buf);
      current_it = current_buf->begin();
      // refill backup buffer, consider async
      std::cout << ">>>>>>> swap buffers" << std::endl;
      refill_backup();
      std::cout << "<<<<<<< swap buffers" << std::endl;
      return true;
    }
    return false;
  }
  virtual BucketInfo* next() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    auto ret = &(*current_it);
    ++current_it;
    return ret;
  }
  virtual int remove() {
    // unimplemented
    return 0;
  }
  BucketIterator(size_t buf_size):  _buf_size(buf_size), _is_refilling(false) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    _buf1.reserve(buf_size);
    _buf2.reserve(buf_size);
    current_buf = &_buf1;
    backup_buf = &_buf2;
    current_it = _buf1.begin();
    init();
  }
private:
  size_t _buf_size;
  std::vector<BucketInfo> _buf1;
  std::vector<BucketInfo> _buf2;
  std::vector<BucketInfo>* current_buf;
  std::vector<BucketInfo>* backup_buf;
  std::vector<BucketInfo>::iterator current_it;

  std::fstream _f;

  bool _is_refilling;

  int refill_backup() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    // do_refill_backup(); // synchronous way
    // async way, fake, useless
//     std::future<void> fut = std::async(std::launch::async,
//                               [this]()->void {
//                                 this->do_refill_backup();
//                               }
//                             );
    // real async way, create a thread
    std::thread th([this]()->void { this->do_refill_backup(); });
    th.detach();
    return 0;
  }

  void do_refill_backup() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    _is_refilling = true;
    backup_buf->clear();
    std::string line;
    size_t line_read = 0;
    while (line_read < _buf_size && getline(_f, line)) {
      std::cout << "+++++++++ id:" << std::atoi(line.substr(0, line.find(" ")).c_str());
      std::cout << " name:" << line.substr(line.find(" ") + 1, line.length()) << std::endl;
      BucketInfo bi;
      bi.id = std::atoi(line.substr(0, line.find(" ")).c_str());
      bi.name = line.substr(line.find(" ") + 1, line.length());
      backup_buf->push_back(std::move(bi));
      ++line_read;
      std::this_thread::sleep_for(std::chrono::milliseconds(110));
    }
    if (line_read == 0) {
      std::cout << "all lines read" << std::endl;
    }
    _is_refilling = false;
  }

  int init() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    _f.open("/users/gavin/tmp/bucket_info.txt");
    if (!_f.is_open()) {
      return -1;
    }
    refill_backup();
    return 0;
  }

public:
  ~BucketIterator() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (_f.is_open()) {
      _f.close();
    }
  }
};

int main(void) {
  BucketIterator bi(10);
  while (bi.has_next()) {
    std::cout << *bi.next() << std::endl;
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return 0;
}

// vim: et tw=100 ts=2 sw=2 cc=100:
