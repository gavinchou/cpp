#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <deque>
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <random>
#include <memory>

class ThreadPool {
public:
  // a lock based sync queue
  std::queue<std::function<void()>> job_queue;
  std::mutex job_queue_lock;

  std::vector<std::thread> worker_thread_group; // multi thread pool
  std::thread worker_thread; // single thread pool
  std::atomic<bool> is_running;

  size_t pool_size;

  ThreadPool(size_t size): is_running(false), pool_size(size) {
    std::cout << "pool size: " << pool_size << std::endl;
  }

  ThreadPool() {
    is_running = false;
    pool_size = std::thread::hardware_concurrency();
  }

  int execute(std::function<void()> f) {
    return submit(f);
  }

  int submit(std::function<void()> f) {
    std::lock_guard<std::mutex> lck(job_queue_lock);
    job_queue.push(f);
    return 0;
  }

  void work() {
    while (is_running) {
      // extract a job
      std::lock_guard<std::mutex> lck(job_queue_lock);
      if (job_queue.empty()) {
        continue;
      }
      auto job = job_queue.front();
      job_queue.pop();
      std::cout << "worker " << std::this_thread::get_id() << " ";
      job();
    }
  }

  int start() {
    is_running = true;

//     std::thread th(&ThreadPool::work, this);
//     worker_thread = std::move(th);

    for (size_t i = 0; i < pool_size; ++i) {
      worker_thread_group.emplace_back(&ThreadPool::work, this);
    }
    return 0;
  }

  int stop() {
    is_running = false;
    return 0;
  }

  ~ThreadPool() {
    is_running = false;
//     if (worker_thread.joinable()) {
//       worker_thread.join();
//     }

    for (auto& i : worker_thread_group) {
      if (i.joinable()) {
        i.join();
      }
    }
  }
};

template <typename E>
class Iterator {
public:
  virtual bool has_next() = 0;
  virtual int remove() = 0;
  virtual E next() = 0;
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
  virtual BucketInfo next() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    auto ret = *current_it;
    ++current_it;
    return ret;
  }
  virtual int remove() {
    // unimplemented
    return 0;
  }

  BucketIterator(size_t buf_size):  _buf_size(buf_size), _is_refilling(false) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    thread_pool = std::make_shared<ThreadPool>(1);
    thread_pool->start();
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
  std::shared_ptr<ThreadPool> thread_pool;

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
//     std::thread th([this]()->void { do_refill_backup(); });
//     th.detach();
    thread_pool->submit([this]()->void { do_refill_backup(); });
    return 0;
  }

  void do_refill_backup() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    _is_refilling = true;
    backup_buf->clear();
    std::string line;
    size_t line_read = 0;
    std::cout << "is open: " << _f.is_open() << std::endl;
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
      std::cout << "failed to open file" << std::endl;
      return -1;
    }
    do_refill_backup();
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

void test_thread_pool() {
  ThreadPool tp(std::thread::hardware_concurrency());
  tp.start();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(100, 200);

  for (int i = 0; i < 150; ++i) {
    auto random_sleep = dis(gen);
    auto start_time = std::chrono::system_clock::now();
    tp.submit([i, random_sleep]()->void {
        std::cout << "job " << i << " sleep for " << random_sleep << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(random_sleep));
      });
    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> elaped_time = end_time - start_time;
    std::cout << "elaped_time: " << elaped_time.count() << std::endl;
  }
}

void test_itertor() {
  BucketIterator bi(10);
  while (bi.has_next()) {
//   while (true) {
//     std::cout << "has next: " << bi.has_next() << std::endl;
    std::cout << std::move(bi.next()) << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

int main(void) {

  test_itertor();
//   test_thread_pool();
  return 0;
}

// vim: et tw=100 ts=2 sw=2 cc=100:
