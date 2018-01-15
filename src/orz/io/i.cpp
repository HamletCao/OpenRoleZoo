//
// Created by lby on 2018/1/15.
//

#include "orz/io/i.h"
#include <fstream>

namespace orz {
    binary read_file(const std::string &filename) {
        binary bin;
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return bin;
        in.seekg(0, std::ios::end);
        auto size = in.tellg();
        bin.resize(size_t(size));
        in.seekg(0, std::ios::beg);
        in.read(bin.data<char>(), bin.size());
        in.close();
        return bin;
    }
}