//
// Created by Lby on 2017/8/12.
//

#ifndef ORZ_MEM_POT_H
#define ORZ_MEM_POT_H

#include <mutex>
#include <memory>

namespace orz {

    class Pot {
    public:
        Pot();

        void *malloc(size_t _size);

        void *relloc(size_t _size);

        template<typename T>
        T *calloc(size_t _count, bool copy = false) {
            if (copy)
                return reinterpret_cast<T *>(this->relloc(sizeof(T) * _count));
            else
                return reinterpret_cast<T *>(this->malloc(sizeof(T) * _count));
        }

        void *data() const;

        size_t capacity() const;

        void clear();

        void swap(Pot &that);

        Pot(const Pot &that);

        Pot &operator=(const Pot &that);

        Pot(Pot &&that);

        Pot &operator=(Pot &&that);

    private:
        size_t m_capacity;
        std::shared_ptr<void> m_data;
    };
}

#endif //ORZ_MEM_POT_H
