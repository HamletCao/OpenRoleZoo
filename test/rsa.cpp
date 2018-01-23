//
// Created by lby on 2018/1/16.
//

#include <orz/codec/rsa.h>
#include <orz/utils/log.h>
#include <cstdlib>

bool rsa_test1(const std::string &bin) {
    std::string codes = orz::rsa_private_encode("orz.key", bin);
    std::string decode_bin = orz::rsa_public_decode("orz_pub.key", codes);
    ORZ_LOG(orz::DEBUG) << bin << " vs. " << decode_bin;
    return decode_bin == bin;
}

bool rsa_test2(const std::string &bin) {
    std::string codes = orz::rsa_public_encode("orz_pub.key", bin);
    std::string decode_bin = orz::rsa_private_decode("orz.key", codes);
    ORZ_LOG(orz::DEBUG) << bin << " vs. " << decode_bin;
    return decode_bin == bin;
}

const std::string random_string() {
    int size = std::rand() % 1024;
    std::string bin(size, 0);
    for (auto &ch : bin) {
        ch = char(std::rand() % 256);
    }
    return std::move(bin);
}

int main(int argc, char *argv[]) {
    std::srand(7726);
    int N = 1024;
    int count = 0;
    for (int i = 0; i < N; ++i) {
        std::string bin = random_string();
        if (i % 2 && rsa_test1(bin)) ++count;
        if (i % 2 == 0 && rsa_test2(bin)) ++count;
    }

    ORZ_LOG(orz::INFO) << "Test count: " << N << ", Succeed count: " << count << ".";

    return 0;
}

