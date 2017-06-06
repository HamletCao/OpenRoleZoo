//
// Created by Lby on 2017/6/1.
//

#ifndef ORZ_UTILS_FORMAT_H
#define ORZ_UTILS_FORMAT_H

#include <string>
#include <sstream>
#include <vector>

namespace orz {
    static void _Concat_str(std::ostream &out) {}

    template<typename T, typename... Args>
    static void _Concat_str(std::ostream &out, const T &t, Args... args) { _Concat_str(out << t, args...); }

    template<typename... Args>
    inline const std::string Concat(Args... args) {
        std::ostringstream oss;
        _Concat_str(oss, args...);
        return std::move(oss.str());
    }

    const std::string Format(const std::string &f);

    std::vector<std::string> Split(const std::string& str, char ch = ' ', size_t _size = 0);
    std::vector<std::string> Split(const std::string& str, const std::string sep = " ", size_t _size = 0);

}

#endif //ORZ_UTILS_FORMAT_H
