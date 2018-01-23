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

namespace orz {
    class rsa_key {
#ifndef WITH_OPENSSL
#else   // !WITH_OPENSSL
    public:
        using inner_type = rsa_st;
        enum key_type {
            PUBLIC,
            PRIVATE
        };

        rsa_key(inner_type *rsa, key_type type) : m_rsa(rsa), m_type(type) {}

        key_type type() const {
            return m_type;
        }

        inner_type *inner() {
            return m_rsa;
        }

        const inner_type *inner() const {
            return m_rsa;
        }

        inner_type *m_rsa = nullptr;
        key_type m_type;

        static rsa_key *load_public(const std::string &filename) {
            FILE *key_file = std::fopen(filename.c_str(), "rb");
            if (key_file == nullptr) return nullptr;
            need close_file(fclose, key_file);
            rsa_st *rsa = PEM_read_RSA_PUBKEY(key_file, nullptr, nullptr, nullptr);
            if (rsa == nullptr) return nullptr;
            return new rsa_key(rsa, PUBLIC);
        }

        static rsa_key *load_private(const std::string &filename) {
            FILE *key_file = std::fopen(filename.c_str(), "rb");
            if (key_file == nullptr) return nullptr;
            need close_file(fclose, key_file);
            rsa_st *rsa = PEM_read_RSAPrivateKey(key_file, nullptr, nullptr, nullptr);
            if (rsa == nullptr) return nullptr;
            return new rsa_key(rsa, PRIVATE);
        }

        static void free(const rsa_key *key) {
            if (key) RSA_free(key->m_rsa);
        }

#endif  // !WITH_OPENSSL
    };

    rsa_key *load_public_rsa_key(const std::string &filename) {
        return rsa_key::load_public(filename);
    }

    rsa_key *load_private_rsa_key(const std::string &filename) {
        return rsa_key::load_private(filename);
    }

    void free_rsa_key(const rsa_key *key) {
        rsa_key::free(key);
    }

    std::string rsa_private_encode(const std::string &filename, const std::string &data) {
        auto key = load_private_rsa_key(filename);
        if (key == nullptr) return std::string();
        need free_key(free_rsa_key, key);

        std::string rdata;
        std::string block;
        size_t block_size = 100;

        try {
            // split and encode
            size_t index = 0;
            while (true) {
                if (index >= data.length()) break;
                block = data.substr(index, block_size);
                if (block.empty()) break;
                block = rsa_encode_block(key, block);
                rdata.insert(rdata.end(), block.begin(), block.end());
                index += block_size;
            }
            return std::move(rdata);
        } catch (const Exception &) {
            return std::string();
        }
    }

    std::string rsa_public_decode(const std::string &filename, const std::string &data) {
        auto key = load_public_rsa_key(filename);
        if (key == nullptr) return std::string();
        need free_key(free_rsa_key, key);

        std::string rdata;
        std::string block;
        size_t block_size = 128;

        try {
            // split and encode
            size_t index = 0;
            while (true) {
                if (index >= data.length()) break;
                block = data.substr(index, block_size);
                if (block.empty()) break;
                block = rsa_decode_block(key, block);
                rdata.insert(rdata.end(), block.begin(), block.end());
                index += block_size;
            }
            return std::move(rdata);
        } catch (const Exception &) {
            return std::string();
        }
    }

    std::string rsa_public_encode(const std::string &filename, const std::string &data) {
        auto key = load_public_rsa_key(filename);
        if (key == nullptr) return std::string();
        need free_key(free_rsa_key, key);

        std::string rdata;
        std::string block;
        size_t block_size = 100;

        try {
            // split and encode
            size_t index = 0;
            while (true) {
                if (index >= data.length()) break;
                block = data.substr(index, block_size);
                if (block.empty()) break;
                block = rsa_encode_block(key, block);
                rdata.insert(rdata.end(), block.begin(), block.end());
                index += block_size;
            }
            return std::move(rdata);
        } catch (const Exception &) {
            return std::string();
        }
    }

    std::string rsa_private_decode(const std::string &filename, const std::string &data) {
        auto key = load_private_rsa_key(filename);
        if (key == nullptr) return std::string();
        need free_key(free_rsa_key, key);

        std::string rdata;
        std::string block;
        size_t block_size = 128;

        try {
            // split and encode
            size_t index = 0;
            while (true) {
                if (index >= data.length()) break;
                block = data.substr(index, block_size);
                if (block.empty()) break;
                block = rsa_decode_block(key, block);
                rdata.insert(rdata.end(), block.begin(), block.end());
                index += block_size;
            }
            return std::move(rdata);
        } catch (const Exception &) {
            return std::string();
        }
    }

    std::string rsa_encode_block(rsa_key *key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        RSA *rsa = key->inner();
        auto rsa_len = RSA_size(rsa);
        std::unique_ptr<char[]> rdata(new char[rsa_len + 1]);
        std::memset(rdata.get(), 0, rsa_len + 1);
        int rsize = 0;
        switch (key->type()) {
            case rsa_key::PUBLIC:
                rsize = RSA_public_encrypt(
                        static_cast<int>(data.size()),
                        reinterpret_cast<const unsigned char *>(data.data()),
                        reinterpret_cast<unsigned char *>(rdata.get()),
                        rsa, RSA_PKCS1_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
                break;
            case rsa_key::PRIVATE:
                rsize = RSA_private_encrypt(
                        static_cast<int>(data.size()),
                        reinterpret_cast<const unsigned char *>(data.data()),
                        reinterpret_cast<unsigned char *>(rdata.get()),
                        rsa, RSA_PKCS1_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
                break;
        }
        if (rsize < 0) {
            ERR_print_errors_fp(stdout);
            ORZ_LOG(ERROR) << "OpenSSL RAS error." << crash;
        }
        return std::string(rdata.get(), static_cast<size_t >(rsize));
#endif
    }

    std::string rsa_decode_block(rsa_key *key, const std::string &data) {
#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL
        RSA *rsa = key->inner();
        auto rsa_len = RSA_size(rsa);
        std::unique_ptr<char[]> rdata(new char[rsa_len + 1]);
        std::memset(rdata.get(), 0, rsa_len + 1);
        int rsize = 0;
        switch (key->type()) {
            case rsa_key::PUBLIC:
                rsize = RSA_public_decrypt(
                        static_cast<int>(data.size()),
                        reinterpret_cast<const unsigned char *>(data.data()),
                        reinterpret_cast<unsigned char *>(rdata.get()),
                        rsa, RSA_PKCS1_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
                break;
            case rsa_key::PRIVATE:
                rsize = RSA_private_decrypt(
                        static_cast<int>(data.size()),
                        reinterpret_cast<const unsigned char *>(data.data()),
                        reinterpret_cast<unsigned char *>(rdata.get()),
                        rsa, RSA_PKCS1_PADDING /* RSA_PKCS1_PADDING RSA_NO_PADDING */);
                break;
        }
        if (rsize < 0) {
            ERR_print_errors_fp(stdout);
            ORZ_LOG(ERROR) << "OpenSSL RAS error." << crash;
        }
        return std::string(rdata.get(), static_cast<size_t >(rsize));
#endif
    }
}
