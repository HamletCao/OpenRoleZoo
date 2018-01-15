//
// Created by lby on 2018/1/15.
//

#include "orz/codec/aes.h"

#include <cstring>
#include <cstdint>
#include <cassert>
#include <memory>

namespace orz {
    class block4x4 {
    public:
        block4x4(char *data) : m_data(data) {}

        // block4x4(const char *data) : m_data(const_cast<char *>(data)) {}

        char &operator()(int row, int col) {
            return m_data[row * 4 + col];
        }

        unsigned char &u(int row, int col) {
            return *(reinterpret_cast<unsigned char *>(&operator()(row, col)));
        }

        uint32_t &row(int n) {
            return *(reinterpret_cast<uint32_t *>(&operator()(n, 0)));
        }

        char *data() { return m_data; }

    private:
        char *m_data;
    };

    static void left_shift_loop(uint32_t &lhs, int rhs) {
        lhs = (lhs << rhs) | (lhs >> (32 - rhs));
    }

    static void right_shift_loop(uint32_t &lhs, int rhs) {
        lhs = (lhs >> rhs) | (lhs << (32 - rhs));
    }

    static void shift_rows_encode(block4x4 block) {
        left_shift_loop(block.row(1), 8);
        left_shift_loop(block.row(2), 16);
        left_shift_loop(block.row(3), 24);
    }

    static void shift_rows_decode(block4x4 block) {
        right_shift_loop(block.row(1), 8);
        right_shift_loop(block.row(2), 16);
        right_shift_loop(block.row(3), 24);
    }

    static unsigned char XTIME(unsigned char x) {
        return static_cast<unsigned char>(((x << 1) ^ ((x & 0x80) ? 0x1b : 0x00)) & 0xff);
    }

    static unsigned char multiply(unsigned char a, unsigned char b) {
        unsigned char temp[8] = {a};
        unsigned char tempmultiply = 0x00;
        int i = 0;
        for (i = 1; i < 8; i++) {
            temp[i] = XTIME(temp[i - 1]);
        }
        tempmultiply = static_cast<unsigned char>(((b & 0x01) * a) & 0xff);
        for (i = 1; i <= 7; i++) {
            tempmultiply ^= (((b >> i) & 0x01) * temp[i]);
        }
        return tempmultiply;
    }

    static char multiply(char a, char b) {
        return static_cast<char>(
                multiply(static_cast<unsigned char>(a),
                         static_cast<unsigned char>(b)));
    }

    // a1 * b1 + a2 * b2 + a3 * b3 + a4 * b4
    static char vector4_mul(
            char a1, char a2, char a3, char a4,
            char b1, char b2, char b3, char b4
    ) {
        return multiply(a1, b1) ^ multiply(a2, b2) ^ multiply(a3, b3) ^ multiply(a4, b4);
    }

    // a * b => c
    static void metrix_mul(block4x4 a, block4x4 b, block4x4 c) {
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                c(row, col) = vector4_mul(
                        a(row, 0),a(row, 1),a(row, 2),a(row, 3),
                        b(0, col),b(1, col),b(2, col),b(3, col)
                );
            }
        }
    }

    static char encode_matrix[] = {
            0x02, 0x03, 0x01, 0x01,
            0x01, 0x02, 0x03, 0x01,
            0x01, 0x01, 0x02, 0x03,
            0x03, 0x01, 0x01, 0x02,
    };

    static char decode_matrix[] = {
            0x0E, 0x0B, 0x0D, 0x09,
            0x09, 0x0E, 0x0B, 0x0D,
            0x0D, 0x09, 0x0E, 0x0B,
            0x0B, 0x0D, 0x09, 0x0E,
    };

    static void mix_columns_encode(block4x4 block) {
        block4x4 encode_block = encode_matrix;
        char result[16];
        block4x4 result_blobk = result;
        metrix_mul(encode_block, block, result_blobk);
        std::memcpy(block.data(), result, 16);
    }

    static void mix_columns_decode(block4x4 block) {
        block4x4 decode_block = decode_matrix;
        char result[16];
        block4x4 result_blobk = result;
        metrix_mul(decode_block, block, result_blobk);
        std::memcpy(block.data(), result, 16);
    }

    std::string aes128_encode(const std::string &bin) {
        std::unique_ptr<char[]> codes(new char[bin.size()]);
        std::memcpy(codes.get(), bin.data(), bin.size());
        shift_rows_encode(codes.get());
        mix_columns_encode(codes.get());
        return std::string(codes.get(), bin.size());
    }

    std::string aes128_decode(const std::string &codes) {
        std::unique_ptr<char[]> bin(new char[codes.size()]);
        std::memcpy(bin.get(), codes.data(), codes.size());
        mix_columns_decode(bin.get());
        shift_rows_decode(bin.get());
        return std::string(bin.get(), codes.size());
    }
}

