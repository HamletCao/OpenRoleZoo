//
// Created by Lby on 2017/8/16.
//

#include "orz/io/jug/jug.h"

#include <fstream>

namespace orz {

    jug::jug()
            : jug(Piece::NIL) {}

    jug::jug(Piece::Type type)
            : m_pie(Piece::Get(type)) {}

    jug::jug(std::nullptr_t _)
            : m_pie(std::make_shared<NilPiece>()) {}

    jug::jug(int val)
            : m_pie(std::make_shared<IntPiece>(val)) {}

    jug::jug(float val)
            : m_pie(std::make_shared<FloatPiece>(val)) {}

    jug::jug(const std::string &val)
            : m_pie(std::make_shared<StringPiece>(val)) {}

    bool jug::valid(Piece::Type type) const {
        return m_pie && m_pie->type() == type;
    }

    bool jug::valid() const {
        return !valid(Piece::NIL);
    }

    jug &jug::operator=(nullptr_t _) {
        switch (m_pie->type()) {
            case Piece::NIL:
                break;
            default:
                m_pie = std::make_shared<NilPiece>();
                break;
        }
        return *this;
    }

    jug &jug::operator=(int val) {
        switch (m_pie->type()) {
            case Piece::INT:
                reinterpret_cast<IntPiece *>(m_pie.get())->set(val);
                break;
            default:
                m_pie = std::make_shared<IntPiece>(val);
                break;
        }
        return *this;
    }

    jug &jug::operator=(float val) {
        switch (m_pie->type()) {
            case Piece::FLOAT:
                reinterpret_cast<FloatPiece *>(m_pie.get())->set(val);
                break;
            default:
                m_pie = std::make_shared<FloatPiece>(val);
                break;
        }
        return *this;
    }

    jug &jug::operator=(const std::string &val) {
        switch (m_pie->type()) {
            case Piece::STRING:
                reinterpret_cast<StringPiece *>(m_pie.get())->set(val);
                break;
            default:
                m_pie = std::make_shared<StringPiece>(val);
                break;
        }
        return *this;
    }

    jug &jug::operator=(const binary &val) {
        switch (m_pie->type()) {
            case Piece::BINARY:
                reinterpret_cast<BinaryPiece *>(m_pie.get())->set(val);
                break;
            default:
                m_pie = std::make_shared<BinaryPiece>(val);
                break;
        }
        return *this;
    }

    jug::operator bool() const {
        return m_pie && m_pie->notnil();
    }

    jug::operator int() const {
        switch (m_pie->type()) {
            case Piece::INT:
                return reinterpret_cast<IntPiece *>(m_pie.get())->get();
            case Piece::FLOAT:
                return static_cast<int>(
                        reinterpret_cast<FloatPiece *>(m_pie.get())->get());
            case Piece::STRING:
                return std::atoi(
                        reinterpret_cast<StringPiece *>(m_pie.get())->get().c_str());
            default:
                throw Exception("Can not convert this jug to int");
        }
    }

    jug::operator float() const {
        switch (m_pie->type()) {
            case Piece::INT:
                return static_cast<float>(
                        reinterpret_cast<IntPiece *>(m_pie.get())->get());
            case Piece::FLOAT:
                return reinterpret_cast<FloatPiece *>(m_pie.get())->get();
            case Piece::STRING:
                return static_cast<float>(
                        std::atof(reinterpret_cast<StringPiece *>(m_pie.get())->get().c_str()));
            default:
                throw Exception("Can not convert this jug to float");
        }
    }

    jug::operator std::string() const {
        switch (m_pie->type()) {
            case Piece::STRING:
                return reinterpret_cast<StringPiece *>(m_pie.get())->get();
            case Piece::BINARY:
            {
                auto bin = reinterpret_cast<BinaryPiece *>(m_pie.get())->get();
                return std::string(bin.data<char>(), bin.size());
            }
            default:
                throw Exception("Can not convert this jug to string");
        }
    }

    jug::operator binary() const {
        switch (m_pie->type()) {
            case Piece::STRING:
            {
                auto &str = reinterpret_cast<StringPiece *>(m_pie.get())->get();
                return binary(str.data(), str.size());
            }
            case Piece::BINARY:
                return reinterpret_cast<BinaryPiece *>(m_pie.get())->get();
            default:
                throw Exception("Can not convert this jug to binary");
        }
    }

    // string, binary, list, dict function
    size_t jug::size() const {
        switch (m_pie->type()) {
            case Piece::STRING:
                return reinterpret_cast<StringPiece *>(m_pie.get())->get().length();
            case Piece::BINARY:
                return reinterpret_cast<BinaryPiece *>(m_pie.get())->size();
            case Piece::LIST:
                return reinterpret_cast<ListPiece *>(m_pie.get())->size();
            case Piece::DICT:
                return reinterpret_cast<DictPiece *>(m_pie.get())->size();
            default:
                throw Exception("This jug has no method size()");
        }
    }

    // list function
    jug jug::index(size_t i) {
        switch (m_pie->type()) {
            case Piece::NIL:
                m_pie = Piece::Get(Piece::LIST);
            case Piece::LIST: {
                auto list = reinterpret_cast<ListPiece *>(m_pie.get());
                if (i < 0 || i >= list->size()) {
                    return jug();
                } else {
                    return list->index(i);
                }
            }
            default:
                throw Exception("This jug has no method index(i)");
        }
    }

    jug jug::index(size_t i, const jug &value) {
        switch (m_pie->type()) {
            case Piece::NIL:
                m_pie = Piece::Get(Piece::LIST);
            case Piece::LIST: {
                auto list = reinterpret_cast<ListPiece *>(m_pie.get());
                if (i < 0 || i < list->size()) {
                    throw Exception("Index out of range");
                } else {
                    return list->index(i, value.m_pie);
                }
            }
            default:
                throw Exception("This jug has no method index(i, value)");
        }
    }

    jug &jug::append(const jug &value) {
        switch (m_pie->type()) {
            case Piece::NIL:
                m_pie = Piece::Get(Piece::LIST);
            case Piece::LIST: {
                auto list = reinterpret_cast<ListPiece *>(m_pie.get());
                list->push_back(value.m_pie);
                return *this;
            }
            default:
                throw Exception("This jug has no method index(i, value)");
        }
    }

    // dict function
    jug jug::index(const std::string &key) {
        switch (m_pie->type()) {
            case Piece::NIL:
                m_pie = Piece::Get(Piece::DICT);
            case Piece::DICT: {
                auto dict = reinterpret_cast<DictPiece *>(m_pie.get());
                if (dict->has_key(key)) {
                    return dict->index(key);
                } else {
                    return jug();
                }
            }
            default:
                throw Exception("This jug has no method index(key)");
        }
    }

    jug jug::index(const std::string &key, const jug &value) {
        switch (m_pie->type()) {
            case Piece::NIL:
                m_pie = Piece::Get(Piece::DICT);
            case Piece::DICT: {
                auto dict = reinterpret_cast<DictPiece *>(m_pie.get());
                return dict->index(key, value.m_pie);
            }
            default:
                throw Exception("This jug has no method index(key, value)");
        }
    }

    std::vector<std::string> jug::keys() const {
        switch (m_pie->type()) {
            case Piece::DICT: {
                auto dict = reinterpret_cast<DictPiece *>(m_pie.get());
                return dict->keys();
            }
            default:
                throw Exception("This jug has no method keys()");
        }
    }

    // binary function
    jug &jug::set_bits(const void *buffer, size_t size) {
        switch (m_pie->type()) {
            case Piece::NIL:
                m_pie = Piece::Get(Piece::BINARY);
            case Piece::BINARY: {
                auto buff = reinterpret_cast<BinaryPiece *>(m_pie.get());
                buff->set_bits(buffer, size);
                return *this;
            }
            default:
                throw Exception("This jug has no method set_bits(buffer, size)");
        }
    }

    jug &jug::push_bits(const void *buffer, size_t size) {
        switch (m_pie->type()) {
            case Piece::NIL:
                m_pie = Piece::Get(Piece::BINARY);
            case Piece::BINARY: {
                auto buff = reinterpret_cast<BinaryPiece *>(m_pie.get());
                buff->push_bits(buffer, size);
                return *this;
            }
            default:
                throw Exception("This jug has no method set_bits(buffer, size)");
        }
    }

    Piece *jug::raw() {
        return m_pie.get();
    }

    const Piece *jug::raw() const {
        return m_pie.get();
    }

    std::ostream &operator<<(std::ostream &out, const jug &e) {
        auto &m_pie = e.m_pie;
        switch (m_pie->type()) {
            case Piece::NIL:
                return out << '\"' << "@nil" << '\"';
            case Piece::INT:
                return out << reinterpret_cast<IntPiece *>(m_pie.get())->get();
            case Piece::FLOAT:
                return out << reinterpret_cast<FloatPiece *>(m_pie.get())->get();
            case Piece::STRING:
                return out << '\"' << reinterpret_cast<StringPiece *>(m_pie.get())->get() << '\"';
            case Piece::BINARY:
                return out << '\"' << "@binary:" << reinterpret_cast<BinaryPiece *>(m_pie.get())->size() << '\"';
            case Piece::LIST: {
                auto list = reinterpret_cast<ListPiece *>(m_pie.get());
                out << '[';
                for (size_t i = 0; i < list->size(); ++i) {
                    if (i) out << ',' << ' ';
                    out << jug(list->index(i));
                }
                out << ']';
                return out;
            }
            case Piece::DICT: {
                auto dict = reinterpret_cast<DictPiece *>(m_pie.get());
                out << '{';
                bool first = true;
                for (auto &key : dict->keys()) {
                    if (!first) out << ',' << ' ';
                    else first = false;
                    out << "\"" << key << "\": " << jug(dict->index(key));
                }
                out << '}';
                return out;
            }
        }

        return out;
    }


    jug jug_parse(const std::string &buffer) {
        std::istringstream iss(buffer, std::ios::binary);
        return Piece::Read(iss);
    }

    std::string jug_build(const jug &j) {
        std::ostringstream oss(std::ios::binary);
        Piece::Write(oss, j.m_pie);
        return oss.str();
    }

    jug jug_read(const std::string &filename) {
        std::ifstream infile(filename, std::ios::binary);
        if (infile.is_open()) {
            return Piece::Read(infile);
        } else {
            return jug();
        }
    }

    jug jug_read(std::istream &in) {
        return Piece::Read(in);
    }

    bool jug_write(const std::string &filename, const jug &j) {
        std::ofstream outfile(filename, std::ios::binary);
        if (outfile.is_open()) {
            Piece::Write(outfile, j.m_pie);
            return true;
        }
        return false;
    }

    void jug_write(std::ostream &out, const jug &j) {
        Piece::Write(out, j.m_pie);
    }

    jug sta_read(const std::string &filename, int mask) {
        std::ifstream infile(filename, std::ios::binary);
        if (infile.is_open()) {
            return sta_read(infile, STA_MASK);
        } else {
            return jug();
        }
    }

    jug sta_read(std::istream &in, int mask) {
        int stream_mask = 0;
        binio<int>::read(in, stream_mask);
        if (stream_mask != mask) return jug();
        return Piece::Read(in);
    }

    bool sta_write(const std::string &filename, const jug &j, int mask) {
        std::ofstream outfile(filename, std::ios::binary);
        if (outfile.is_open()) {
            sta_write(outfile, j, STA_MASK);
            return true;
        }
        return false;
    }

    void sta_write(std::ostream &out, const jug &j, int mask) {
        binio<int>::write(out, mask);
        Piece::Write(out, j.m_pie);
    }

}
