//
// Created by lby on 2018/1/10.
//

#include "orz/codec/json.h"

#include "json_iterator.h"

namespace orz {
    static bool is_space(char ch) {
        return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
    }

    static json_iterator jump_space(json_iterator it) {
        while (it != it.end() && is_space(*it)) ++it;
        return it;
    }

    static bool parse_null(json_iterator &beg, jug &value) {
        beg = jump_space(beg);
        if (beg == beg.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json to null" << crash;
        if (beg.cut(beg + 4) == "null") {
            beg += 4;
            value = nullptr;
            return true;
        }
        return false;
    }

    static jug parse_boolean(json_iterator &beg) {
        beg = jump_space(beg);
        if (beg == beg.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json to boolean" << crash;
        jug result;
        if (beg.cut(beg + 4) == "true"){
            beg += 4;
            result = true;
        }else if (beg.cut(beg + 5) == "false") {
            beg += 5;
            result = false;
        }
        return result;
    }

    int char2hex(char ch) {
        int lch = std::tolower(ch);
        if ('0' <= lch && lch <= '9') return lch - '0';
        if ('a' <= lch && lch <= 'f') return lch - 'a' + 10;
        return -1;
    }

    static std::string parse_string(json_iterator &beg) {
        beg = jump_space(beg);
        if (beg == beg.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json to string" << crash;
        if (*beg != '"') ORZ_LOG(ERROR) << "syntax error: string begin with " << *beg << crash;
        std::string value;
        auto it = beg;
        bool slant = false;
        int unicode_index = 0;
        char unicode = 0;
        while (++it != it.end()) {
            if (unicode_index > 0) {
                int ch = char2hex(*it);
                if (ch < 0) ORZ_LOG(ERROR) << "syntax error: unrecognized unicode" << crash;
                switch (unicode_index) {
                    case 1:
                        unicode |= (ch << 4);
                        unicode_index++;
                        break;
                    case 2:
                        unicode |= ch;
                        value.push_back(char(unicode));
                        unicode = 0;
                        unicode_index++;
                        break;
                    case 3:
                        unicode |= (ch << 4);
                        unicode_index++;
                        break;
                    case 4:
                        unicode |= ch;
                        value.push_back(char(unicode));
                        unicode = 0;
                        unicode_index = 0;
                        break;
                    default:
                        break;
                }
                continue;
            } else if (slant) {
                switch (*it) {
                    case '\"':
                        value.push_back(*it);
                        break;
                    case '\\':
                        value.push_back(*it);
                        break;
                    case '/':
                        value.push_back(*it);
                        break;
                    case 'b':
                        value.push_back('\b');
                        break;
                    case 'f':
                        value.push_back('\f');
                        break;
                    case 'n':
                        value.push_back('\n');
                        break;
                    case 'r':
                        value.push_back('\r');
                        break;
                    case 't':
                        value.push_back('\t');
                        break;
                    case 'u':
                        unicode_index = 1;
                        break;
                    default:
                        value.push_back(*it);
                        break;
                }
                slant = false;
                continue;
            } else if (*it == '\\') {
                slant = true;
                continue;
            } else if (*it == '"') {
                beg = it + 1;
                return std::move(value);
            }
            value.push_back(*it);
        }
        ORZ_LOG(ERROR) << "syntax error: can not find match \"" << crash;
        return std::string();
    }

    static jug parse_number(json_iterator &beg) {
        beg = jump_space(beg);
        if (beg == beg.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json to number" << crash;
        jug result;
        const char *number_c_string = &(*beg);
        char *end_ptr = nullptr;
        double value = std::strtod(number_c_string, &end_ptr);
        if (end_ptr == number_c_string) return result;
        auto ivalue = static_cast<int>(value);
        if (double(ivalue) == value) result = ivalue;
        else result = value;
        beg += end_ptr - number_c_string;
        return result;
    }

    static jug parse_value(json_iterator &beg);

    static jug parse_list(json_iterator &beg) {
        beg = jump_space(beg);
        if (beg == beg.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json to list" << crash;
        if (*beg != '[') ORZ_LOG(ERROR) << "syntax error: list begin with " << *beg << crash;
        jug value(Piece::LIST);
        auto it = beg;
        while (++it != it.end()) {
            it = jump_space(it);
            if (it == it.end() || *it == ']') break;
            jug local_value = parse_value(it);
            value.append(local_value);
            it = jump_space(it);
            if (it != it.end() && *it == ',') continue;
            break;
        }
        if (it == it.end() || *it != ']') ORZ_LOG(ERROR) << "syntax error: can not find match ]" << crash;
        beg = it + 1;
        return std::move(value);
    }

    static jug parse_dict(json_iterator &beg) {
        beg = jump_space(beg);
        if (beg == beg.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json to dict" << crash;
        if (*beg != '{') ORZ_LOG(ERROR) << "syntax error: dict begin with " << *beg << crash;
        jug value(Piece::DICT);
        auto it = beg;
        while (++it != it.end()) {
            it = jump_space(it);
            if (it == it.end() || *it == '}') break;
            std::string local_key = parse_string(it);
            it = jump_space(it);
            if (it == it.end() || *it != ':') ORZ_LOG(ERROR) << "syntax error: dict key:value must split with :" << crash;
            ++it;
            jug local_value = parse_value(it);
            value.index(local_key, local_value);
            it = jump_space(it);
            if (it != it.end() && *it == ',') continue;
            break;
        }
        if (it == it.end() || *it != '}') ORZ_LOG(ERROR) << "syntax error: can not find match }" << crash;
        beg = it + 1;
        return std::move(value);
    }

    /// TODO: add bool support
    static jug parse_value(json_iterator &beg) {
        beg = jump_space(beg);
        if (beg == beg.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json" << crash;
        jug value;
        auto it = beg;
        value = parse_number(beg);
        if (value.valid()) return value;
        if (*it == '"') return parse_string(beg);
        if (*it == '[') return parse_list(beg);
        if (*it == '{') return parse_dict(beg);
        value = parse_boolean(beg);
        if (value.valid()) return value;
        if (parse_null(beg, value)) return value;
        ORZ_LOG(ERROR) << "syntax error: unrecognized symbol " << *it << crash;
        return jug();
    }

    jug json2jug(const std::string &json) {
        json_iterator it(json.c_str(), static_cast<int>(json.length()));
        return parse_value(it);
    }

    std::string jug2json(const orz::jug &obj) {
        return obj.repr();
    }

    std::string form_encode(const orz::jug &obj) {
        if (!obj.valid(Piece::DICT)) ORZ_LOG(ERROR) << "form encoding only supporting dict" << crash;
        std::ostringstream oss;
        int first = true;
        for (auto &key : obj.keys()) {
            if (first) first = false;
            else oss << "&";
            oss << key << "=" << obj[key].str();
        }
        return oss.str();
    }

}
