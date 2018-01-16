//
// Created by lby on 2018/1/15.
//

#ifndef ORZ_CODEC_AES_H
#define ORZ_CODEC_AES_H

#include <string>

namespace orz {
    enum CRYPTO_MODE {
        CBC
    };

    std::string
    aes128_encode(const std::string &key, CRYPTO_MODE mode, const std::string &data, const std::string &iv = "");

    std::string
    aes128_decode(const std::string &key, CRYPTO_MODE mode, const std::string &data, const std::string &iv = "");
}


#endif //ORZ_CODEC_AES_H
