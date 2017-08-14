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
#include <future>

namespace orz {

    class Canyon {
    public:
        using Operation = std::function<void()>;

        explicit Canyon(int size = -1);

        ~Canyon();

        template<typename FUNC, typename... Args>
        void operator()(FUNC func, Args... args) const {
            auto op = [func, args...]() {
                func(args...);
            };
            this->push(op);
        };

        void join() const;

    private:
        Canyon(const Canyon &that) = delete;

        const Canyon &operator=(const Canyon &that) = delete;

        void push(const Operation &op) const;

        void operating() const;

        mutable std::queue<Operation> _task;
        mutable std::mutex _mutex;
        mutable std::condition_variable _cond;
        std::atomic<bool> _work;
        int _size;

        std::thread _core;
    };

}

#endif //ORZ_SYNC_CANYON_H
