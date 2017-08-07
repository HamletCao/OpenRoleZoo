//
// Created by Lby on 2017/8/7.
//

#ifndef ORZ_IO_DIR_H
#define ORZ_IO_DIR_H

#include <string>

namespace orz {
    const std::string FileSeparator();

    bool mkdir(const std::string &dir);

    bool access(const std::string &path);
}

#endif //ORZ_IO_DIR_H
