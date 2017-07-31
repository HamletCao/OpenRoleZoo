//
// Created by Lby on 2017/6/6.
//

#ifndef ORZ_UTILS_EXCEPT_H
#define ORZ_UTILS_EXCEPT_H

#include <exception>
#include <string>

namespace orz
{
    class Exception : std::exception
    {
    public:
        Exception(const std::string &message);
        virtual const char* what() const noexcept override;
    private:
        std::string m_message;
    };
}

#endif //ORZ_UTILS_EXCEPT_H
