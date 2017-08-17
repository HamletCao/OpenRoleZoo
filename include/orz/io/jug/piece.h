//
// Created by Lby on 2017/8/16.
//

#ifndef ORZ_IO_JUG_PIECE_H
#define ORZ_IO_JUG_PIECE_H

#include "orz/utils/except.h"

#include <iostream>
#include <memory>
#include <vector>
#include <map>

namespace orz {

    class Piece {
    public:
        enum Type {
            NIL = 0,
            INT = 1,
            FLOAT = 2,
            STRING = 3,
            BINARY = 4,
            LIST = 5,
            DICT = 6
        };

        Piece(Type type)
                : m_type(type) {}

        virtual ~Piece() {}

        virtual std::istream &read(std::istream &bin) = 0;

        virtual std::ostream &write(std::ostream &bin) const = 0;

        static inline void Write(std::ostream &bin, const Piece &pie);

        static inline std::shared_ptr<Piece> Read(std::istream &bin);

    public:

        Type type() const {
            return m_type;
        }

        operator bool() const {
            return this->m_type != NIL;
        }

        bool nil() const {
            return this->m_type == NIL;
        }

        bool notnil() const {
            return this->m_type != NIL;
        }

    private:
        Type m_type;
    };

    template<Piece::Type _type>
    class TypedPiece : public Piece {
    public:
        TypedPiece() : Piece(_type) {}
    };

    template<typename T>
    T defualt_value() {
        return T();
    }

    template<typename T>
    class binio {
    public:
        static std::ostream &write(std::ostream &bin, const T &elem) {
            /// TODO: check big or little endian
            return bin.write(reinterpret_cast<const char *>(&elem), sizeof(T));
        }

        static std::istream &read(std::istream &bin, T &elem) {
            /// TODO: check big or little endian
            return bin.read(reinterpret_cast<char *>(&elem), sizeof(T));
        }
    };

    template<>
    class binio<std::string> {
    public:
        static std::ostream &write(std::ostream &bin, const std::string &str) {
            int size = static_cast<int>(str.size());
            binio<int>::write(bin, size);
            bin.write(str.data(), str.size());
            return bin;
        }

        static std::istream &read(std::istream &bin, std::string &str) {
            int size;
            binio<int>::read(bin, size);
            std::unique_ptr<char[]> buffer(new char[size]);
            bin.read(buffer.get(), size);
            str = std::string(buffer.get(), size);
            return bin;
        }
    };

    template<Piece::Type _type, typename T>
    class ValuedPiece : public TypedPiece<_type> {
    public:
        ValuedPiece() : m_val(defualt_value<T>()) {}

        ValuedPiece(const T &val) : m_val(val) {}

        ValuedPiece(std::istream &bin) {
            this->read(bin);
        }

        ValuedPiece &set(const T &val) {
            this->m_val = val;
            return *this;
        }

        T &get() {
            return this->m_val;
        }

        const T &get() const {
            return this->m_val;
        }

        virtual std::istream &read(std::istream &bin) override {
            return binio<T>::read(bin, m_val);
        }

        virtual std::ostream &write(std::ostream &bin) const override {
            binio<char>::write(bin, static_cast<char>(this->type()));
            binio<T>::write(bin, m_val);
            return bin;
        }

    private:
        T m_val;
    };

    using NilPiece = ValuedPiece<Piece::NIL, char>;
    using IntPiece = ValuedPiece<Piece::INT, int>;
    using FloatPiece = ValuedPiece<Piece::FLOAT, float>;
    using StringPiece = ValuedPiece<Piece::STRING, std::string>;
    using BinaryPiece = ValuedPiece<Piece::BINARY, std::string>;

    class ListPiece : public TypedPiece<Piece::LIST> {
    public:
        ListPiece() {}

        ListPiece(std::istream &bin) {
            this->read(bin);
        }

        size_t size() const {
            return m_list.size();
        }

        void push_back(const std::shared_ptr<Piece> &pie) {
            m_list.push_back(pie);
        }

        void clear() {
            m_list.clear();
        }

        const std::shared_ptr<Piece> &operator[](size_t i) const {
            return m_list[i];
        }

        std::shared_ptr<Piece> &operator[](size_t i) {
            return m_list[i];
        }

        virtual std::istream &read(std::istream &bin) override {
            int size;
            binio<int>::read(bin, size);
            m_list.reserve(size);
            for (int i = 0; i < size; ++i) {
                m_list.push_back(Piece::Read(bin));
            }
            return bin;
        }

        virtual std::ostream &write(std::ostream &bin) const override {
            binio<char>::write(bin, static_cast<char>(this->type()));
            binio<int>::write(bin, static_cast<int>(this->size()));
            for (auto &pie : m_list) {
                pie->write(bin);
            }
            return bin;
        }

    private:
        std::vector<std::shared_ptr<Piece>> m_list;
    };

    class DictPiece : public TypedPiece<Piece::DICT> {
    public:
        DictPiece() {}

        DictPiece(std::istream &bin) {
            this->read(bin);
        }

        size_t size() const {
            return m_dict.size();
        }

        size_t erase(const std::string &key) {
            return m_dict.erase(key);
        }

        template <size_t _size>
        std::shared_ptr<Piece> &operator[](const char (&key)[_size]) {
            return this->operator[](std::string(key));
        }

        std::shared_ptr<Piece> &operator[](const std::string &key) {
            return m_dict[key];
        }

        virtual std::istream &read(std::istream &bin) override {
            int size;
            binio<int>::read(bin, size);
            m_dict.clear();
            for (int i = 0; i < size; ++i) {
                std::string key;
                binio<std::string>::read(bin, key);
                m_dict[key] = Piece::Read(bin);
            }
            return bin;
        }

        virtual std::ostream &write(std::ostream &bin) const override {
            binio<char>::write(bin, static_cast<char>(this->type()));
            binio<int>::write(bin, static_cast<int>(this->size()));
            for (auto &key_pie : m_dict) {
                auto &key = key_pie.first;
                auto &pie = key_pie.second;
                binio<std::string>::write(bin, key);
                pie->write(bin);
            }
            return bin;
        }

    private:
        std::map<std::string, std::shared_ptr<Piece>> m_dict;
    };

    void Piece::Write(std::ostream &bin, const Piece &pie) {
        pie.write(bin);
    }

    std::shared_ptr<Piece> Piece::Read(std::istream &bin) {
        char type;
        binio<char>::read(bin, type);
        switch (static_cast<Type>(type)) {
            case NIL:
                return std::make_shared<NilPiece>(bin);
            case INT:
                return std::make_shared<IntPiece>(bin);
            case FLOAT:
                return std::make_shared<FloatPiece>(bin);
            case STRING:
                return std::make_shared<StringPiece>(bin);
            case BINARY:
                return std::make_shared<BinaryPiece>(bin);
            case LIST:
                return std::make_shared<ListPiece>(bin);
            case DICT:
                return std::make_shared<DictPiece>(bin);
        }
        throw Exception("Unknown piece type.");
    }
}


#endif //ORZ_IO_JUG_PIECE_H
