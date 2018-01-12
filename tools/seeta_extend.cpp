//
// Created by Lby on 2017/8/18.
//

#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include "orz/io/jug/piece.h"

std::string read_binary_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    file.seekg(0, file.end);
    auto size = file.tellg();
    file.seekg(0, file.beg);
    std::unique_ptr<char[]> buffer(new char[size]);
    file.read(buffer.get(), size);
    return std::string(buffer.get(), size);
}

void write_binary_file(const std::string &filename, const std::string &buff) {
    std::ofstream file(filename, std::ios::binary);
    file.write(buff.data(), buff.size());
}

void write_binary_stream(std::ostream &file, const std::string &buff) {
    file.write(buff.data(), buff.size());
}

int main(int argc, char *argv[]) {

    if (argc < 8) {
        std::cout <<
             "Usage:" <<
             "seeta_extend original_model extended_model feature_size channels width height blob_name" <<
             std::endl;
        return EXIT_FAILURE;
    }

    std::string original_model = argv[1];
    std::string extended_model = argv[2];
    int feature_size = atoi(argv[3]);
    int channels = atoi(argv[4]);
    int width = atoi(argv[5]);
    int height = atoi(argv[6]);
    std::string blob_name = argv[7];

    std::string model = read_binary_file(original_model);

    if (model.size() == 0) {
        std::cerr << "Can not open file" << original_model << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outfile(extended_model, std::ios::binary);
    orz::binio<int>::write(outfile, feature_size);
    orz::binio<int>::write(outfile, channels);
    orz::binio<int>::write(outfile, width);
    orz::binio<int>::write(outfile, height);
    orz::binio<std::string>::write(outfile, blob_name);

    write_binary_stream(outfile, model);

    outfile.close();

    std::cout << original_model << " ==> " << extended_model << std::endl;

    return EXIT_SUCCESS;
}