#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <condition_variable>
#include <mutex>

#include "simple_sync_queue.hpp"

template <typename JobType = std::function<void()>>
class ThreadPool {
public:
  // a lock based sync queue
  std::shared_ptr<SimpleSyncQueue<JobType>> job_queue;

  std::vector<std::thread> worker_thread_group; // multi thread pool
  std::atomic<bool> is_running;
  size_t pool_size;

  ThreadPool(size_t size): is_running(false), pool_size(size) {
    job_queue = std::make_shared<SimpleSyncQueue<JobType>>(10);
  }

  ThreadPool() = delete;

  /**
   * Submits jobs, if job queue is full it will blocked
   *
   */
  int submit(JobType f) {
    if (!is_running) {
      // not started
      return -1;
    }
    job_queue->put(f);
    return 0;
  }

  /**
   * Working thread
   */
  void work() {
    while (is_running || !job_queue->empty()) {
      JobType job;
      if (!job_queue->get_with_timeout(&job, 10)) {
        continue;
      }
      std::cout << "worker " << std::this_thread::get_id() << " ";
      job();
    }
  }

  int start() {
    is_running = true;
    worker_thread_group.clear();
    for (size_t i = 0; i < pool_size; ++i) {
      worker_thread_group.emplace_back(&ThreadPool::work, this);
    }
    return 0;
  }

  /**
   * Stops to get jobs from job queue, the job being done will finish normally
   *
   * @return 0 if succeed, otherwise non-zero value returned
   */
  int stop() {
    if (!is_running) {
      // already stopped
      return -1;
    }
    is_running = false;
    for (auto& i : worker_thread_group) {
      if (i.joinable()) {
        i.join();
      }
    }
    return 0;
  }

  /**
   * Terminates the thread pool immediately, jobs in the queue will not be done
   * and the running threads will be detached
   *
   * @return 0 if succeed, otherwise non-zero value returned
   */
  int terminate() {
    if (!is_running) {
      // not running
      return -1;
    }
    is_running = false;
    job_queue.clear(); // abandon all jobs
    for (auto& i : worker_thread_group) {
      i.detach();
    }
    return 0;
  }

  ~ThreadPool() {
    if (is_running) {
      stop();
    }
  }
};

// vim: et tw=80 ts=2 sw=2 cc=80:
