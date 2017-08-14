//
// Created by Lby on 2017/8/12.
//

#ifndef ORZ_MEM_POOL_H
#define ORZ_MEM_POOL_H

#include <mutex>
#include <memory>

namespace orz {

    class MemoryPot {
    public:
        void *alloc(size_t _size);
        const void *alloc(size_t _size) const;

        template <typename T>
        T *alloc(size_t _count) {
            return reinterpret_cast<T*>(this->alloc(sizeof(T) * _count));
        }

        template <typename T>
        const T *alloc(size_t _count) const {
            return reinterpret_cast<T*>(this->alloc(sizeof(T) * _count));
        }

        void free() const;
    private:
        size_t _capacity;
        std::shared_ptr<void> _data;
    };
}

#endif //ORZ_MEM_POOL_H
