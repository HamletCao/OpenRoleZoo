//
// Created by Lby on 2017/8/23.
//

#include <orz/utils/except.h>
#include "orz/mem/vat.h"

namespace orz {

    Vat::Vat() {
    }

    void *Vat::malloc(size_t _size) {
        // find unroped pot
        for (size_t i = 0; i < m_list.size(); ++i) {
            if (m_list[i].roped) continue;
            m_list[i].roped = true;
            void *ptr = m_list[i].pot.malloc(_size);
            m_dict.insert(std::make_pair(ptr, i));
            return ptr;
        }
        RopedPot p;
        p.roped = true;
        void *ptr = p.pot.malloc(_size);
        m_list.push_back(p);
        m_dict.insert(std::make_pair(ptr, m_list.size() - 1));
        return ptr;
    }

    void Vat::free(const void *ptr) {
        auto key = const_cast<void *>(ptr);
        auto it = m_dict.find(key);
        if (it == m_dict.end()) {
            throw orz::Exception("Can not free this ptr");
        }
        m_list[it->second].roped = false;
        m_dict.erase(key);
    }

    void Vat::reset() {
        for (auto &pair : m_dict) {
            m_list[pair.second].roped = false;
        }
        m_dict.clear();
    }

    void Vat::dispose() {
        m_dict.clear();
        m_list.clear();
    }

    void Vat::swap(Vat &that)
    {
        this->m_list.swap(that.m_list);
        this->m_dict.swap(that.m_dict);
    }

    Vat::Vat(Vat &&that)
    {
        this->swap(that);
    }

    Vat &Vat::operator=(Vat &&that)
    {
        this->swap(that);
        return *this;
    }
}