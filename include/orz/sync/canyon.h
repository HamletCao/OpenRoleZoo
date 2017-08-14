//
// Created by Lby on 2017/8/11.
//

#ifndef ORZ_SYNC_CANYON_H
#define ORZ_SYNC_CANYON_H

#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <queue>

namespace orz {

    template<typename T>
    class Canyon {
    public:
        using Operation = std::function<void(const T &)>;

        template<typename OP>
        explicit Canyon(OP op, int size = -1)
                : _work(true), _size(size) {
            this->_op = [op](const T &arg) { op(arg); };
            this->_core = std::thread(&Canyon::operating, this);
        }

        ~Canyon() {
            this->join();
            _work = false;
            _cond.notify_all();
            _core.join();
        }

        void operator()(const T &arg) const {
            std::unique_lock<std::mutex> _locker(_mutex);
            while (_size > 0 && _task.size() >= static_cast<size_t >(_size)) _cond.wait(_locker);
            _task.push(arg);
            _cond.notify_all();
        }

        void join() const {
            std::unique_lock<std::mutex> _locker(_mutex);
            while (_task.size()) _cond.wait(_locker);
        }

    private:
        Canyon(const Canyon &that) = delete;

        const Canyon &operator=(const Canyon &that) = delete;

        void operating() const {
            std::unique_lock<std::mutex> _locker(_mutex);
            while (_work) {
                while (_work && _task.size() == 0) _cond.wait(_locker);
                if (!_work) break;
                T arg = _task.front();
                _task.pop();
                _op(arg);
                _cond.notify_all();
            }
        }

        Operation _op;
        mutable std::queue<T> _task;
        mutable std::mutex _mutex;
        mutable std::condition_variable _cond;
        std::atomic<bool> _work;
        int _size;

        std::thread _core;
    };

}

#endif //ORZ_SYNC_CANYON_H
