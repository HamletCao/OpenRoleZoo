//
// Created by Lby on 2017/6/1.
//

#include "orz/utils/format.h"

namespace orz {
    const std::string Format(const std::string &f) {
        return f;
    }

    std::vector<std::string> Split(const std::string &str, char ch, size_t _size) {
        std::vector<std::string> result;
        std::string::size_type left = 0, right;

        result.reserve(_size);
        while (true) {
            right = str.find(ch, left);
            result.push_back(str.substr(left, right == std::string::npos ? std::string::npos : right - left));
            if (right == std::string::npos) break;
            left = right + 1;
        }
        return std::move(result);
    }

    std::string::size_type FindDecollator(const std::string &str, const std::string &sep, std::string::size_type off) {
        if (off == std::string::npos) return std::string::npos;
        std::string::size_type i = off;
        for (; i < str.length(); ++i) {
            if (sep.find(str[i]) != std::string::npos) return i;
        }
        return std::string::npos;
    }

    std::vector<std::string> Split(const std::string &str, const std::string sep, size_t _size) {
        std::vector<std::string> result;
        std::string::size_type left = 0, right;

        result.reserve(_size);
        while (true) {
            right = FindDecollator(str, sep, left);
            result.push_back(str.substr(left, right == std::string::npos ? std::string::npos : right - left));
            if (right == std::string::npos) break;
            left = right + 1;
        }
        return std::move(result);
    }

    std::string Join(const std::vector<std::string>& list, const std::string &sep) {
        std::ostringstream oss;
        for (int i = 0; i < list.size(); ++i) {
            if (i) oss << sep;
            oss << list[i];
        }
        return oss.str();
    }
}
