//
// Created by Lby on 2017/6/1.
//

#ifndef ORZ_FORMAT_H
#define ORZ_FORMAT_H

#include <string>
#include <sstream>

namespace orz {
    inline void _Concat_str(std::ostream &out) {}

    template<typename T, typename... Args>
    inline void _Concat_str(std::ostream &out, const T &t, Args... args) { _Concat_str(out << t, args...); }

    template<typename... Args>
    inline const std::string Concat(Args... args) {
        std::ostringstream oss;
        _Concat_str(oss, args...);
        return std::move(oss.str());
    }

    const std::string Format(const std::string &f);
}

#endif //ORZ_FORMAT_H
