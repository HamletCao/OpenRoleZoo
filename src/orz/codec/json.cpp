//
// Created by lby on 2018/1/10.
//

#include "orz/codec/json.h"

#include "json_iterator.h"

namespace orz
{
    static std::string parse_string(json_iterator &it)
    {
        if (it == it.end()) ORZ_LOG(ERROR) << "syntax error: converting empty json to string" << crash;
        if (*it != '"') ORZ_LOG(ERROR) << "syntax error: string begin with " << *it << crash;
        std::ostringstream oss;
        while (++it != it.end())
        {
            if (*it == '"')
            {
                ++it;
                return oss.str();
            }
            oss << *it;
        }
        ORZ_LOG(ERROR) << "syntax error: can not find match \"" << crash;
        return std::string();
    }

    static jug parse_num(json_iterator &it){
        return jug();
    }

    jug json2jug(const std::string &json) {
        json_iterator it(json.data(), static_cast<int>(json.size()));
        ORZ_LOG(INFO) << parse_string(it);
        return orz::jug();
    }

    std::string jug2json(const orz::jug &obj) {
        return std::string();
    }

}
