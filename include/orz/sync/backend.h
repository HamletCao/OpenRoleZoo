//
// Created by lby on 2018/6/5.
//

#ifndef ORZ_SYNC_BACKEND_H
#define ORZ_SYNC_BACKEND_H

#include "cartridge.h"

namespace orz {
    template<typename T>
    class Backend {
    public:
        using Mission = std::function<T()>;

        T fire(const Mission &mission) {
            this->put(mission);
            return this->get();
        }

        bool busy() const { return this->m_busy; }

        void put(const Mission &mission) {
            if (!this->m_busy) {
                this->m_busy = true;
                this->m_cart.fire(0, [this, mission](int) {
                    T a = mission();
                    std::unique_lock<std::mutex> _locker(this->m_mutex);
                    this->m_future = std::move(a);
                }, [this](int) {
                    this->m_busy = false;
                });
            }
        }

        T get() const {
            std::unique_lock<std::mutex> _locker(this->m_mutex);
            return this->m_future;
        }

    private:
        mutable std::mutex m_mutex;
        T m_future;
        orz::Cartridge m_cart;
        mutable std::atomic<bool> m_busy = false;
    };
}


#endif //ORZ_SYNC_BACKEND_H
