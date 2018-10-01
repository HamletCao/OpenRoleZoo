//
// Created by kier on 2018/10/2.
//

#include "orz/utils/uuid.h"

#include "orz/utils/platform.h"

#if ORZ_PLATFORM_OS_MAC || ORZ_PLATFORM_OS_IOS || ORZ_PLATFORM_OS_LINUX

#include <sstream>
#include <uuid/uuid.h>
#include <iomanip>
#include <cstring>

std::string orz::uuid() {
    uuid_t uuid;
    uuid_generate(uuid);
    unsigned char *p = uuid;

    std::ostringstream oss;
    if (sizeof(uuid_t) == 16) {
        char uuid_str[37];
        snprintf(uuid_str, sizeof(uuid_str),
                 "%02X%02X%02X%02X_%02X%02X_%02X%02X_%02X%02X_%02X%02X%02X%02X%02X%02X",
                 p[0], p[1], p[2], p[3],
                 p[4], p[5],
                 p[6], p[7],
                 p[8], p[9],
                 p[10], p[11], p[12], p[13], p[14], p[15]);
        oss << uuid_str;
    } else {
        oss << std::hex;
        oss << std::setfill('0');
        for (size_t i = 0; i < sizeof(uuid_t); i++, p++) {
            oss << std::setw(2) << (int(*p) & 0xff);
        }
    }

    return oss.str();
}

#elif ORZ_PLATFORM_OS_WINDOWS

#include <objbase.h>
#include <cstring>

std::string orz::uuid() {
    GUID guid;
    CoCreateGuid(&guid);
    char uuid_str[37];
    snprintf(uuid_str, sizeof(uuid_str),
             "%02X%02X%02X%02X_%02X%02X_%02X%02X_%02X%02X_%02X%02X%02X%02X%02X%02X",
             guid.Data1, guid.Data2, guid.Data3,
             guid.Data4[0], guid.Data4[1],
             guid.Data4[2], guid.Data4[3],
             guid.Data4[4], guid.Data4[5],
             guid.Data4[6], guid.Data4[7]);
    return uuid_str;
}

#endif

