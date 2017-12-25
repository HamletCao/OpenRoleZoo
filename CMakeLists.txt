cmake_minimum_required(VERSION 3.3)
project(ORZ)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

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

add_library(${PROJECT_NAME}_SHARED SHARED ${SRC_FILES})
add_library(${PROJECT_NAME}_STATIC STATIC ${SRC_FILES})
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
    set_target_properties(test_${file} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LOCAL_OUTPUT_DIR}/lib)
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
install(FILES ${INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/orz)

if (WIN32)
    install(FILES ${LOCAL_OUTPUT_DIR}/lib/lib${PROJECT_NAME}.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif ()