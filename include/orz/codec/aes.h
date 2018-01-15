//
// Created by lby on 2018/1/15.
//

#ifndef ORZ_CODEC_AES_H
#define ORZ_CODEC_AES_H

#include <string>

namespace orz
{
    std::string aes128_encode(const std::string &bin);
    std::string aes128_decode(const std::string &codes);
}


#endif //ORZ_CODEC_AES_H
