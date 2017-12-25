//
// Created by Lby on 2017/10/31.
//

#ifndef ORZ_IO_JUG_BINARY_H
#define ORZ_IO_JUG_BINARY_H

#include <cstddef>
#include <memory>

namespace orz {
    class binary {
    public:
        using self = binary;

        enum class pos
        {
            now,
            beg,
            end,
        };

        using size_t = ::size_t;

        // return size
        size_t size() const;

        // return capacity
        size_t capacity() const;

        // read _size bytes into _buffer from binary, and index += _size
        size_t read(void *_buffer, size_t _size) const;

        // write _size bytes from _buffer into binary, and index += _size
        size_t write(const void *_buffer, size_t _size);

        // return index
        size_t get_pos() const;

        // move index,
        // index = case now: index + _shift
        //         case beg: _shift
        //         case end: size + _shift
        size_t set_pos(pos _pos, int _shift);

        // move index, index += _size
        void shift(int _size);

        const void *data() const;
        void *data();

        template <typename T>
        const T *data() const {return reinterpret_cast<T*>(data());}

        template <typename T>
        T *data() {return reinterpret_cast<T*>(data());}

        // return a clone of this binary
        binary clone() const;

        // set size memory to ch
        void memset(char ch);

        // set part of memory to ch
        void memset(pos _pos, int _begin, int _end, char ch);

        // prepare _size memory, do not change size and index
        void reverse(size_t _size);

        // reverse, then change size. do not change index
        void resize(size_t _size);

        // reset size and index
        void clear();

        // clear and free the data
        void dispose();

    private:
        std::shared_ptr<void> m_data;
        size_t m_capacity = 0;
        size_t m_size = 0;
        mutable size_t m_index = 0;

        size_t correct_index(int _index);
        size_t correct_index(pos _pos, int _shift);
        void *now_data();
        const void *now_data() const;
    };
}


#endif //ORZ_IO_JUG_BINARY_H
