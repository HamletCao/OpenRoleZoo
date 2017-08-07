#include "orz/io/dir.h"

#include "orz/utils/format.h"
#include "orz/utils/platform.h"
#include <cstdio>

#if ORZ_PLATFORM_OS_WINDOWS

#include <direct.h>
#include <io.h>

#define ACCESS ::_access
#define MKDIR(a) ::_mkdir((a))

#elif  ORZ_PLATFORM_OS_LINUX || ORZ_PLATFORM_OS_MAC

#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

#define ACCESS ::access
#define MKDIR(a) ::mkdir((a),0755)

#endif

namespace orz {

    const std::string FileSeparator() {
#if ORZ_PLATFORM_OS_WINDOWS
        return "\\";
#else
        return "/";
#endif
    }

    bool mkdir_core(const std::string &dir) {
        int miss = ACCESS(dir.c_str(), 0);
        if (miss) {
            int failed = MKDIR(dir.c_str());
            if (failed) {
                return false;
            }
        }
        return true;
    }

    bool mkdir(const std::string &dir) {
        auto path = orz::Split(dir, "\\/");
        for (int i = 1; i <= path.size(); ++i) {
            if (path[i - 1].empty()) continue;
            auto local_path = orz::Join(std::vector<std::string>(path.begin(), path.begin() + i), FileSeparator());
            if (!mkdir_core(local_path)) return false;
        }
        return true;
    }

    bool access(const std::string &path) {
        int miss = ACCESS(path.c_str(), 0);
        return !miss;
    }

    bool remove(const std::string &filename) {
        return std::remove(filename.c_str()) == 0;
    }

    bool rename(const std::string &oldname, const std::string &newname) {
        return std::rename(oldname.c_str(), newname.c_str()) == 0;
    }
}

