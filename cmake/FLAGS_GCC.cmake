# GCC flags

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -std=c++11")
set(CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} -fPIC")
set(CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_MODULE} -fPIC")
# set(CMAKE_CXX_CREATE_STATIC_LIBRARY "${CMAKE_CXX_CREATE_STATIC_LIBRARY}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

if (${PLATFORM} STREQUAL "x86")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
elseif (${PLATFORM} STREQUAL "x64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
endif()

if (${CONFIGURATION} STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -ggdb")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -ggdb")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()
