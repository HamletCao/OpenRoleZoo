//
// Created by Lby on 2017/8/16.
//

#ifndef ORZ_IO_JUG_JUG_H
#define ORZ_IO_JUG_JUG_H

#include "piece.h"

namespace orz {

    /*
     * @brief interface of piece
     */
    class jug {
    public:

        jug();

        jug(Piece::Type type);

        jug(std::nullptr_t);

        jug(int val);

        jug(unsigned int val) : jug(static_cast<int>(val)) {}

        jug(char val) : jug(static_cast<int>(val)) {}

        jug(unsigned char val) : jug(static_cast<int>(val)) {}

        jug(long val) : jug(static_cast<int>(val)) {}

        jug(unsigned long val) : jug(static_cast<int>(val)) {}

        jug(long long val) : jug(static_cast<int>(val)) {}

        jug(unsigned long long val) : jug(static_cast<int>(val)) {}

        jug(float val);

        jug(double val) : jug(static_cast<float>(val)) {}

        jug(const std::string &val);

        template<size_t _size>
        jug(const char (&val)[_size]) : jug(std::string(val)) {}

        jug(const char *val) : jug(std::string(val)) {}

        bool valid(Piece::Type type);

        // common type set function

        // common type function
        operator bool() const;

        operator int() const;

        operator float() const;

        operator std::string() const;

        // string, binary, list, dict function
        size_t size() const;

        // list function
        jug index(size_t i);

        jug operator[](size_t i) {
            return index(i);
        }

        jug index(size_t i, const jug &value);

        jug &append(const jug &value);

        // dict function
        jug index(const std::string &key);

        jug operator[](const std::string &key) {
            return index(key);
        }

        jug index(const std::string &key, const jug &value);

        std::vector<std::string> keys() const;

        // binary function
        jug &set_bits(void *buffer, size_t size);

        jug &push_bits(void *buffer, size_t size);

        template<typename T>
        jug &push_bits(const T &val) {
            switch (m_pie->type()) {
                case Piece::BINARY: {
                    auto buff = reinterpret_cast<BinaryPiece *>(m_pie.get());
                    buff->push_bits<T>(val);
                    return *this;
                }
                default:
                    throw Exception("This jug has no method push_bits<T>(val)");
            }
        }

    private:
        jug(const std::shared_ptr<Piece> &pie)
                : m_pie(pie) {}

        operator std::shared_ptr<Piece>() const {
            return m_pie;
        }

        std::shared_ptr<Piece> m_pie;

    public:

        friend std::ostream &operator<<(std::ostream &out, const jug &e);

        friend jug jug_parse(const std::string &buffer);

        friend std::string jug_build(const jug &j);

        friend jug jug_read(const std::string &filename);

        friend jug jug_read(std::istream &in);

        friend bool jug_write(const std::string &filename, const jug &j);

        friend void jug_write(std::ostream &out, const jug &j);

    };

    std::ostream &operator<<(std::ostream &out, const jug &e);

    jug jug_parse(const std::string &buffer);

    std::string jug_build(const jug &j);

    jug jug_read(const std::string &filename);

    jug jug_read(std::istream &in);

    bool jug_write(const std::string &filename, const jug &j);

    void jug_write(std::ostream &out, const jug &j);
}


#endif //ORZ_IO_JUG_JUG_H