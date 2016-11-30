/**
  * @brief  defines an iterator interface
  * @author zhoufei
  * @email  
  * @date   2016-10-28-Fri
 */

#ifndef DEFINE_GUARD_ITERATOR_HPP
#define DEFINE_GUARD_ITERATOR_HPP

#include <memory>
#include <iostream>

template<typename E>
class Iterator {
public:
    /**
     * Returns the next element in the iteration.
     *
     * @return the next element in the iteration
     */
    virtual std::shared_ptr<E> next() = 0;

    /**
     * Returns true if the iteration has more elements. (In other words, returns true if next would
     * return an element rather than throwing an exception.)
     *
     * @return true if the iteration has more elements
     */
    virtual bool has_next() = 0;

    /**
     * Removes from the underlying collection the last element returned by this iterator (optional
     * operation). This method can be called only once per call to next. The behavior of an iterator
     * is unspecified if the underlying collection is modified while the iteration is in progress in
     * any way other than by calling this method.
     *
     * @return 0 if removal succeeded
     */
    virtual int remove() = 0;

    virtual ~Iterator() {};
};

#include <condition_variable>
#include <thread>
#include <vector>

/**
 * Double buffered iterator, **not thread safe**, some memeber fucntions need to be impmented by the
 * sub-class. Internal buffers are lock-free while reading and writing.
 *
 */
template<typename E>
class DoubleBufferedIterator: public Iterator<E> {
protected:
    size_t _buf_size;
    bool _is_refilling;
    std::vector<std::shared_ptr<E>> _buf1;
    std::vector<std::shared_ptr<E>> _buf2;
    std::vector<std::shared_ptr<E>>* _current_buf;
    std::vector<std::shared_ptr<E>>* _backup_buf;
    typename std::vector<std::shared_ptr<E>>::iterator _current_it;

private:
    std::shared_ptr<std::thread> _refill_thread;
    std::mutex _looper_mtx;
    std::condition_variable _looper_cv;
    bool _looper_quit;

public:
    /**
     * Checks if there are more elements in both buffers and swaps the backup bufere and with
     * current buffer accordingly
     */
    virtual bool has_next() {
        if (_current_it != _current_buf->end()) {
            // current buffer is not empty
            return true;
        }
        while (_is_refilling) {
            // TODO: optimize with condition variable
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (_current_it == _current_buf->end()
                && _backup_buf->size() > 0) {
            // swap buffer
            _current_buf->clear();
            std::swap(_current_buf, _backup_buf);
            _current_it = _current_buf->begin();
            // refill backup buffer, consider async
            refill_backup();
            return true;
        }
        return false;
    }

    virtual std::shared_ptr<E> next() {
        if (_current_it == _current_buf->end()) {
            return nullptr;
        }
        auto ret = *_current_it;
        ++_current_it;
        return ret;
    }

    virtual int remove() {
        // unimplemented
        return 0;
    }

    DoubleBufferedIterator(size_t buf_size): _buf_size(buf_size), _is_refilling(false) {
        _buf1.reserve(buf_size);
        _buf2.reserve(buf_size);
        _current_buf = &_buf1;
        _backup_buf = &_buf2;
        _current_it = _buf1.begin();
        _looper_quit = false;
        _refill_thread = std::make_shared<std::thread>([this]{ refill_looper(); });
    }

    /**
     * Initiates the iterator, **must be called explicitly in sub-class** once the
     * sub-class is ready for call of `do_refill_backup()` to ensure the iterator initiated
     * correctly
     *
     * TODO: add a flag to indicate whether the iterator has been inited
     *
     * @return always retun 0
     */
    int init() {
        do_refill_backup();
        return 0;
    }

protected:
    /**
     * Sub-class needs to implement this function to refill the buffer
     * note that the sub class has to concern about the buffer size while refilling the backup
     * buffer, be aware of making the buffer oversized
     *
     */
    virtual void do_refill_backup() = 0;

private:
    int refill_backup() {
        _looper_cv.notify_one();
        return 0;
    }

    void refill_looper() {
        while (!_looper_quit) {
            std::unique_lock<std::mutex> lck(_looper_mtx);
            _looper_cv.wait(lck);
            if (_looper_quit) {
                break;
            }
            _is_refilling = true;
            do_refill_backup();
            _is_refilling = false;
        }
    }

public:
    ~DoubleBufferedIterator() {
        _looper_quit = true;
        _looper_cv.notify_all();
        _refill_thread->join();
    }
};

#endif // DEFINE_GUARD_ITERATOR_HPP

// vim: et tw=100 ts=4 sw=4 cc=100:
