//
// Created by lby on 2018/1/16.
//

#ifndef ORZ_RSA_H
#define ORZ_RSA_H

#include <string>

namespace orz
{
    std::string ras_private_encode(const std::string &private_key, const std::string &data);
    std::string ras_public_decode(const std::string &public_key, const std::string &data);
    std::string ras_public_encode(const std::string &public_key, const std::string &data);
    std::string ras_private_decode(const std::string &private_key, const std::string &data);
}

#endif //ORZ_RSA_H
