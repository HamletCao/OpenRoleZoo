//
// Created by lby on 2018/1/16.
//

#include "orz/codec/rsa.h"
#include "orz/utils/log.h"

#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <memory>
#include <cstring>
#include "orz/mem/need.h"

#endif  // !WITH_OPENSSL

namespace orz
{
    std::string rsa_private_encode(const std::string &private_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        RSA *rsa = nullptr;
        FILE *key_file = std::fopen(private_key.c_str(), "rb");
        if (key_file == nullptr) ORZ_LOG(ERROR) << "can not open key: " << private_key << crash;
        need close_file(fclose, key_file);
        // ERR_print_errors_fp(stdout);
        rsa = PEM_read_RSAPrivateKey(key_file, nullptr, nullptr, nullptr);
        if (rsa == nullptr) ORZ_LOG(ERROR) << "can not read key: " << private_key << crash;
        need free_rsa(RSA_free, rsa);
        auto rsa_len = RSA_size(rsa);
        std::unique_ptr<char[]> rdata(new char[data.length()]);
        std::memset(rdata.get(), 0, data.length());
        auto rsize = RSA_private_encrypt(
                static_cast<int>(data.size()),
                reinterpret_cast<const unsigned char *>(data.data()),
                reinterpret_cast<unsigned char *>(rdata.get()),
                rsa, RSA_NO_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
        if (rsize < 0) {
            ERR_print_errors_fp(stdout);
            ORZ_LOG(ERROR) << "OpenSSL RAS error." << crash;
        }
        return std::string(rdata.get(), static_cast<size_t >(rsize));
#endif  // !WITH_OPENSSL
    }

    std::string rsa_public_decode(const std::string &public_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        RSA *rsa = nullptr;
        FILE *key_file = std::fopen(public_key.c_str(), "rb");
        if (key_file == nullptr) ORZ_LOG(ERROR) << "can not open key: " << public_key << crash;
        need close_file(fclose, key_file);
        // ERR_print_errors_fp(stdout);
        rsa = PEM_read_RSA_PUBKEY(key_file, nullptr, nullptr, nullptr);
        // rsa = PEM_read_RSAPublicKey(key_file, nullptr, nullptr, nullptr);
        if (rsa == nullptr) {
            ERR_print_errors_fp(stdout);
            ORZ_LOG(ERROR) << "can not read key: " << public_key << crash;
        }
        need free_rsa(RSA_free, rsa);
        auto rsa_len = RSA_size(rsa);
        std::unique_ptr<char[]> rdata(new char[rsa_len]);
        std::memset(rdata.get(), 0, rsa_len);
        auto rsize = RSA_public_decrypt(
                static_cast<int>(data.size()),
                reinterpret_cast<const unsigned char *>(data.data()),
                reinterpret_cast<unsigned char *>(rdata.get()),
                rsa, RSA_PKCS1_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
        if (rsize < 0) {
            ERR_print_errors_fp(stdout);
            ORZ_LOG(ERROR) << "OpenSSL RAS error." << crash;
        }
        return std::string(rdata.get(), static_cast<size_t >(rsize));
#endif  // !WITH_OPENSSL
    }

    std::string rsa_public_encode(const std::string &public_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        RSA *rsa = nullptr;
        FILE *key_file = std::fopen(public_key.c_str(), "rb");
        if (key_file == nullptr) ORZ_LOG(ERROR) << "can not open key: " << public_key << crash;
        need close_file(fclose, key_file);
        // ERR_print_errors_fp(stdout);
        rsa = PEM_read_RSA_PUBKEY(key_file, nullptr, nullptr, nullptr);
        // rsa = PEM_read_RSAPublicKey(key_file, nullptr, nullptr, nullptr);
        if (rsa == nullptr) ORZ_LOG(ERROR) << "can not read key: " << public_key << crash;
        need free_rsa(RSA_free, rsa);
        auto rsa_len = RSA_size(rsa);
        std::unique_ptr<char[]> rdata(new char[data.length()]);
        std::memset(rdata.get(), 0, data.length());
        auto rsize = RSA_public_encrypt(
                static_cast<int>(data.size()),
                reinterpret_cast<const unsigned char *>(data.data()),
                reinterpret_cast<unsigned char *>(rdata.get()),
                rsa, RSA_NO_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
        if (rsize < 0) {
            ERR_print_errors_fp(stdout);
            ORZ_LOG(ERROR) << "OpenSSL RAS error." << crash;
        }
        return std::string(rdata.get(), static_cast<size_t >(rsize));
#endif  // !WITH_OPENSSL
    }

    std::string rsa_private_decode(const std::string &private_key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        RSA *rsa = nullptr;
        FILE *key_file = std::fopen(private_key.c_str(), "rb");
        if (key_file == nullptr) ORZ_LOG(ERROR) << "can not open key: " << private_key << crash;
        need close_file(fclose, key_file);
        // ERR_print_errors_fp(stdout);
        rsa = PEM_read_RSAPrivateKey(key_file, nullptr, nullptr, nullptr);
        if (rsa == nullptr) ORZ_LOG(ERROR) << "can not read key: " << private_key << crash;
        need free_rsa(RSA_free, rsa);
        auto rsa_len = RSA_size(rsa);
        std::unique_ptr<char[]> rdata(new char[data.length()]);
        std::memset(rdata.get(), 0, data.length());
        auto rsize = RSA_private_decrypt(
                static_cast<int>(data.size()),
                reinterpret_cast<const unsigned char *>(data.data()),
                reinterpret_cast<unsigned char *>(rdata.get()),
                rsa, RSA_NO_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
        if (rsize < 0) {
            ERR_print_errors_fp(stdout);
            ORZ_LOG(ERROR) << "OpenSSL RAS error." << crash;
        }
        return std::string(rdata.get(), static_cast<size_t >(rsize));
#endif  // !WITH_OPENSSL
    }
}
