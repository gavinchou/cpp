#include <deque>
#include <mutex>
#include <memory>
#include <chrono>
#include <limits>
#include <cstdint>
#include <functional>
#include <condition_variable>

template<typename T,
        template<typename ELEM, typename ALLOC = std::allocator<ELEM>> class CONT = std::deque>
class SimpleSyncQueue {
public:
    SimpleSyncQueue() = default;
    explicit SimpleSyncQueue(std::uint32_t max_size): _max_size(max_size){}
    virtual ~SimpleSyncQueue() {
        _queue.clear();
    }

    void put(const T &t) {
        std::unique_lock<std::mutex> locker(_mutex);
        _not_full.wait(locker, [this](){return _queue.size() < _max_size;});
        _queue.push_back(t);
        _not_empty.notify_one();
    }

    bool put_with_timeout(const T &t, int timeout/*in milliseconds*/) {
        std::unique_lock<std::mutex> locker(_mutex);
        if(_not_full.wait_for(locker,
                            std::chrono::milliseconds(timeout),
                            [this](){return _queue.size() < _max_size;})){
            _queue.push_back(t);
            _not_empty.notify_one();
            return true;
        }
        //timeout
        return false;
    }

    template<typename... Args >
    void emplace(Args&&... args) {
        std::unique_lock<std::mutex> locker(_mutex);
        _not_full.wait(locker, [this](){return _queue.size() < _max_size;});
        _queue.emplace_back(std::forward<Args>(args)...);
        _not_empty.notify_one();
    }

    template<typename... Args >
    bool emplace_with_timeout(int timeout/*in milliseconds*/, Args&&... args) {
        std::unique_lock<std::mutex> locker(_mutex);
        if(_not_full.wait_for(locker,
                            std::chrono::milliseconds(timeout),
                            [this](){return _queue.size() < _max_size;})){
            _queue.emplace_back(std::forward<Args>(args)...);
            _not_empty.notify_one();
            return true;
        }
        //timeout
        return false;
    }

    void get(T *t) {
        std::unique_lock<std::mutex> locker(_mutex);
        _not_empty.wait(locker, [this](){return !_queue.empty();});
        (*t) = std::move(_queue.front());
        _queue.pop_front();
        _not_full.notify_one();
    }

    bool get_with_timeout(T *t, int timeout/*in milliseconds*/) {
        std::unique_lock<std::mutex> locker(_mutex);
        if(_not_empty.wait_for(locker,
                            std::chrono::milliseconds(timeout),
                            [this](){return !_queue.empty();})){
            (*t) = std::move(_queue.front());
            _queue.pop_front();
            _not_full.notify_one();
            return true;
        }
        //timeout
        return false;
    }

    void clear() {
        std::lock_guard<std::mutex> locker(_mutex);
        _queue.clear();
    }

    size_t size() {
        std::lock_guard<std::mutex> locker(_mutex);
        return _queue.size();
    }

    bool empty() {
        std::lock_guard<std::mutex> locker(_mutex);
        return _queue.empty();
    }
private:
    CONT<T> _queue;
    std::mutex _mutex;
    std::condition_variable _not_empty;
    std::condition_variable _not_full;
    std::uint32_t _max_size = std::numeric_limits<std::uint32_t>::max();
};

// vim: et ts=4 sw=4 tw=100:
