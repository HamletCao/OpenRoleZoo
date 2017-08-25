//
// Created by Lby on 2017/8/23.
//

#ifndef ORZ_MEM_VAT_H
#define ORZ_MEM_VAT_H

#include "pot.h"
#include <vector>
#include <map>

namespace orz {

    class Vat {
    public:
        Vat();

        void *malloc(size_t _size);

        template<typename T>
        T *calloc(size_t _count) {
            return reinterpret_cast<T *>(this->malloc(sizeof(T) * _count));
        }

        void free(const void *ptr);

        /**
         * @brief doing like free all malloc ptrs
         */
        void reset();

        void dispose();

        void swap(Vat &that);

        Vat(Vat &&that);

        Vat &operator=(Vat &&that);

    private:
        Vat(const Vat &that) = delete;

        Vat &operator=(const Vat &that) = delete;

        struct RopedPot
        {
            bool roped = false;
            Pot pot;
        };

        std::vector<RopedPot> m_list;
        std::map<void *, size_t> m_dict;    ///< save pair of pointer and index
    };

}


#endif //ORZ_MEM_VAT_H
