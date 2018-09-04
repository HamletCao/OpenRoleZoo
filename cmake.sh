#/usr/bin/env bash

BASE=$(cd `dirname $0`; pwd)

cmake "$BASE" -DCMAKE_TOOLCHAIN_FILE="$BASE"/iOS.cmake -DORZ_WITH_OPENSSL=ON -DOPENSSL_INCLUDE_DIR="$BASE"/../OpenSSL-for-iPhone/include
# -DIOS_PLATFOMR=OS
# -DIOS_PLATFORM=SIMULATOR64
