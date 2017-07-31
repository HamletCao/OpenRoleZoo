#pragma once

#include <vector>
#include <string>

namespace orz
{
const std::string FileSeparator();
std::vector<std::string> FindFiles(const std::string &path);
std::vector<std::string> FindFiles(const std::string &path, std::vector<std::string> &dirs);
std::vector<std::string> FindFilesRecursively(const std::string &path, int depth = -1);
}

