//
// Created by lby on 2018/1/16.
//

#include "orz/codec/rsa.h"

#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL

#include <openssl/rsa.h>
#include <memory>
#include <cstring>

#endif  // !WITH_OPENSSL

namespace orz
{
    std::string ras_private_encode(const std::string &private_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        return std::string();
#endif  // !WITH_OPENSSL
    }

    std::string ras_public_decode(const std::string &public_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        return std::string();
#endif  // !WITH_OPENSSL
    }

    std::string ras_public_encode(const std::string &public_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        return std::string();
#endif  // !WITH_OPENSSL
    }

    std::string ras_private_decode(const std::string &private_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        return std::string();
#endif  // !WITH_OPENSSL
    }
}
