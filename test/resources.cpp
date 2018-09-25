//
// Created by kier on 2018/9/25.
//

#include <orz/tools/resources.h>
#include <sstream>
#include <fstream>
#include <orz/utils/random.h>

static std::string in_source =
        "/file1:file1\n"
        "/file2:file2\n";

static std::string in_binary =
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        "abcdefghigklmn"
        ;

std::string random_string(size_t length) {
    const char letter[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const size_t letter_size = sizeof(letter) / sizeof(letter[0]) - 1;
    std::string rstr(length, '\0');
    for (auto &ch : rstr) {
        ch = letter[orz::random.next(0, letter_size - 1)];
    }
    return std::move(rstr);
}

std::string random_string() {
    return random_string(size_t(orz::random.next(1, 16)));
}

int main() {
    std::istringstream iss_source(in_source);
    std::istringstream iss_binary(in_binary);

    std::ifstream resources_file("in.src.txt");

    orz::resources::compiler compiler;
    std::ofstream header("orz_resources.h");
    std::ofstream source("orz_resources.c");
    compiler.compile(resources_file, header, source);

    // test hash table
    int M = 1000;
    int N = 1000;
    orz::random.seed(4481);
    int count = 0;
    for (int i = 0; i < M; ++i) {
        orz::resources::resources_has_table table;
        std::vector<std::string> rstr(N);
        for (int j = 0; j < N; ++j) {
            rstr[j] = random_string();
        }
        for (int j = 0; j < N; ++j) {
            table.insert(rstr[j]);
        }
        for (int j = 0; j < N; ++j) {
            auto &str = rstr[j];
            auto res = table.find(str);
            if (res) {
                // std::cout << str << " vs. " << res->key << std::endl;
                if (str != res->key) {
                    std::cout << "error type 0 in i = " << i << ", j = " << j << std::endl;
                    ++count;
                }
            } else {
                // std::cout << str << " vs. " << std::endl;
                std::cout << "error type 1 in i = " << i << ", j = " << j << std::endl;
                ++count;
            }
        }
    }
    std::cout << "failed rate: " << float(count) / (M * N) << std::endl;

    return 0;
}

