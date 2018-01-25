cmake_minimum_required(VERSION 2.7)
project(ORZ)

# global root dir
set(SOLUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DORZ_SOLUTION_DIR="${SOLUTION_DIR}")

# dir for common cmake files
list(APPEND CMAKE_MODULE_PATH ${SOLUTION_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${SOLUTION_DIR}/cmake)

# option for platform
set(PLATFORM "auto" CACHE STRING "auto, x86 or x64")
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Debug or Release")
set(CONFIGURATION ${CMAKE_BUILD_TYPE})

# set common compiler flags
include(LOCAL_FLAGS)

option(WITH_OPENSSL "Supporting AES, RAS and SSL with OpenSSL" ON)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/build CACHE STRING "set install prefix" FORCE)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

FILE(GLOB_RECURSE INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/include/orz/*.h
        ${PROJECT_SOURCE_DIR}/include/orz/*.hh
        ${PROJECT_SOURCE_DIR}/include/orz/*.hpp
        )
FILE(GLOB_RECURSE SRC_FILES
        ${PROJECT_SOURCE_DIR}/src/*.c
        ${PROJECT_SOURCE_DIR}/src/*.cc
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        )
FILE(GLOB_RECURSE SRC_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/src/*.h
        ${PROJECT_SOURCE_DIR}/src/*.hh
        ${PROJECT_SOURCE_DIR}/src/*.hpp
        )
# project files for clion
FILE(GLOB_RECURSE TEST_FILES
        ${PROJECT_SOURCE_DIR}/test/*.c
        ${PROJECT_SOURCE_DIR}/test/*.cc
        ${PROJECT_SOURCE_DIR}/test/*.cpp
        )
FILE(GLOB_RECURSE TOOL_FILES
        ${PROJECT_SOURCE_DIR}/tools/*.c
        ${PROJECT_SOURCE_DIR}/tools/*.cc
        ${PROJECT_SOURCE_DIR}/tools/*.cpp
        )

set(LOCAL_OUTPUT_DIR ${PROJECT_SOURCE_DIR})

add_library(${PROJECT_NAME}_STATIC STATIC ${INCLUDE_FILES} ${SRC_INCLUDE_FILES} ${SRC_FILES})
add_library(${PROJECT_NAME}_SHARED SHARED ${INCLUDE_FILES} ${SRC_INCLUDE_FILES} ${SRC_FILES})
# target_link_libraries(${PROJECT_NAME}_SHARED ${PROJECT_NAME}_STATIC)

if (WITH_OPENSSL)
    message(STATUS "Build with OpenSSL")
    add_definitions(-DWITH_OPENSSL)
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME}_SHARED ${OPENSSL_CRYPTO_LIBRARY})
    target_link_libraries(${PROJECT_NAME}_STATIC ${OPENSSL_CRYPTO_LIBRARY})
    target_link_libraries(${PROJECT_NAME}_SHARED ${OPENSSL_SSL_LIBRARY})
    target_link_libraries(${PROJECT_NAME}_STATIC ${OPENSSL_SSL_LIBRARY})
endif (WITH_OPENSSL)

if (WIN32)
    target_link_libraries(${PROJECT_NAME}_SHARED wininet.lib)
    target_link_libraries(${PROJECT_NAME}_STATIC wininet.lib)
endif ()
if (UNIX)
    target_link_libraries(${PROJECT_NAME}_SHARED pthread)
    target_link_libraries(${PROJECT_NAME}_STATIC pthread)
endif ()

set_target_properties(${PROJECT_NAME}_SHARED PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_STATIC PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_static)

if (LOCAL_OUTPUT_DIR)
    set(EXECUTABLE_OUTPUT_PATH ${LOCAL_OUTPUT_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${LOCAL_OUTPUT_DIR}/lib)
endif ()

foreach (path ${TEST_FILES})
    string(REGEX MATCH "[^/]*.[(c)|(cc)|(cpp)]$" file_ext ${path})
    string(REGEX MATCH "^[^.]*" file ${file_ext})
    add_executable(test_${file} ${path})
    target_link_libraries(test_${file} ${PROJECT_NAME}_STATIC)
    # set_target_properties(test_${file} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LOCAL_OUTPUT_DIR}/lib)
endforeach ()

foreach (path ${TOOL_FILES})
    string(REGEX MATCH "[^/]*.[(c)|(cc)|(cpp)]$" file_ext ${path})
    string(REGEX MATCH "^[^.]*" file ${file_ext})
    add_executable(tool_${file} ${path})
    set_target_properties(tool_${file} PROPERTIES OUTPUT_NAME ${file})
    target_link_libraries(tool_${file} ${PROJECT_NAME}_STATIC)
    install(TARGETS tool_${file} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endforeach ()

install(TARGETS ${PROJECT_NAME}_SHARED DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS ${PROJECT_NAME}_STATIC DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/orz)

if (WIN32)
    install(FILES ${LOCAL_OUTPUT_DIR}/lib/lib${PROJECT_NAME}.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif ()
