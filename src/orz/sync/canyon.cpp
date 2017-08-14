//
// Created by Lby on 2017/8/11.
//

#include "orz/sync/canyon.h"

namespace orz {

    Canyon::Canyon(int size)
            : _work(true), _size(size) {
        this->_core = std::thread(&Canyon::operating, this);
    }

    Canyon::~Canyon() {
        this->join();
        _work = false;
        _cond.notify_all();
        _core.join();
    }

    void Canyon::join() const {
        std::unique_lock<std::mutex> _locker(_mutex);
        while (_task.size()) _cond.wait(_locker);
    }

    void Canyon::push(const Operation &op) const {
        std::unique_lock<std::mutex> _locker(_mutex);
        while (_size > 0 && _task.size() >= static_cast<size_t >(_size)) _cond.wait(_locker);
        _task.push(op);
        _cond.notify_all();
    }

    void Canyon::operating() const {
        std::unique_lock<std::mutex> _locker(_mutex);
        while (_work) {
            while (_work && _task.size() == 0) _cond.wait(_locker);
            if (!_work) break;
            auto func = _task.front();
            _task.pop();
            func();
            _cond.notify_all();
        }
    }
}
