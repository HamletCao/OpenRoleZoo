//
// Created by Lby on 2017/8/12.
//

#include "orz/mem/pot.h"

namespace orz {

    Pot::Pot()
            : m_capacity(0), m_data() {
    }

    void *Pot::malloc(size_t _size) {
        if (_size > m_capacity) {
            m_data.reset(std::malloc(_size), [](void *p) { std::free(p); });
            m_capacity = _size;
        }
        return m_data.get();
    }

    void *Pot::relloc(size_t _size) {
        if (_size > m_capacity) {
            void *new_data = std::malloc(_size);
#if _MSC_VER >= 1600
            memcpy_s(new_data, _size, m_data.get(), m_capacity);
#else
            memcpy(new_data, m_data.get(), m_capacity);
#endif
            m_data.reset(new_data, [](void *p) { std::free(p); });
            m_capacity = _size;
        }
        return m_data.get();
    }

    void *Pot::data() const {
        return m_data.get();
    }

    size_t Pot::capacity() const {
        return m_capacity;
    }

    void Pot::clear() {
        m_capacity = 0;
        m_data.reset();
    }

    void Pot::swap(Pot &that) {
        std::swap(this->m_data, that.m_data);
        std::swap(this->m_capacity, that.m_capacity);
    }

    Pot::Pot(const Pot &that)
            : m_capacity(that.m_capacity), m_data(that.m_data) {

    }


    Pot &Pot::operator=(const Pot &that) {
        this->m_data = that.m_data;
        this->m_capacity = that.m_capacity;
        return *this;
    }

    Pot::Pot(Pot &&that)
            : m_capacity(that.m_capacity), m_data(std::move(that.m_data)) {
    }

    Pot &Pot::operator=(Pot &&that) {
        this->swap(that);
        return *this;
    }
}
