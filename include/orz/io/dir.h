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

    /***
     * @brief Remove a file for now
     * @param filename File to remove
     * @return Return 0 only if succeed.
     */
    bool remove(const std::string &filename);

    /***
     * @brief Rename a file
     * @param oldname File's old name
     * @param newname File's new name
     * @return Return 0 only if succeed.
     */
    bool rename(const std::string &oldname, const std::string &newname);
}

#endif //ORZ_IO_DIR_H
