//
// Created by Lby on 2017/10/31.
//

#include "orz/io/jug/binary.h"

#include <cstring>

namespace orz {

    size_t binary::size() const {return m_size;}

    size_t binary::read(void *_buffer, size_t _size) const
    {
        size_t memory_left = m_size - m_index;
        size_t can_read = std::min<size_t>(memory_left, _size);
        std::memcpy(_buffer, now_data(), can_read);
        m_index += can_read;
        return can_read;
    }

    size_t binary::write(const void *_buffer, size_t _size)
    {
        size_t memory_right = m_index + _size;
        reverse(memory_right);
        std::memcpy(now_data(), _buffer, _size);
        if (memory_right > m_size) m_size = memory_right;
        return _size;
    }

    size_t binary::get_pos() const
    {
        return m_index;
    }

//    size_t binary::set_pos(pos _pos, int _shift);
//
//    void binary::shift(int _size);

    const void *binary::data() const
    {
        return m_data.get();
    }

    void *binary::data()
    {
        return m_data.get();
    }

//    binary binary::clone() const;
//
//    void binary::memset(char ch);
//
//    void binary::memset(pos _pos, int _begin, int _end, char ch);

    void binary::reverse(size_t _size)
    {

    }

//    void binary::resize(size_t _size);
//
//    void binary::clear();
//
//    void binary::dispose();

    void *binary::now_data() { return data<char>() + m_index; }

    const void *binary::now_data() const  { return const_cast<self *>(this)->now_data(); }


}