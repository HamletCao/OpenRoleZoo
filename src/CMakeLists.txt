project(ORZ)

set(ORZ_SOURCE_DIR ${PROJECT_SOURCE_DIR}/..)

option(ORZ_WITH_OPENSSL "Supporting AES, RAS and SSL with OpenSSL" OFF)

include_directories(${ORZ_SOURCE_DIR}/include)
include_directories(${ORZ_SOURCE_DIR}/src)

set(ORZ_MODULES codec io lego mem net sync tools utils)

if (ORZ_WITH_OPENSSL)
    LIST(APPEND ORZ_MODULES ssl)
endif (ORZ_WITH_OPENSSL)

foreach (module ${ORZ_MODULES})
    FILE(GLOB_RECURSE INCLUDE_${module}_FILES ${ORZ_SOURCE_DIR}/include/orz/${module}/*.h)
    LIST(APPEND INCLUDE_FILES ${INCLUDE_${module}_FILES})

    FILE(GLOB_RECURSE SRC_${module}_FILES ${ORZ_SOURCE_DIR}/src/orz/${module}/*.cpp)
    LIST(APPEND SRC_FILES ${SRC_${module}_FILES})

    FILE(GLOB_RECURSE SRC_INCLUDE_${module}_FILES ${ORZ_SOURCE_DIR}/src/orz/${module}/*.h)
    LIST(APPEND SRC_INCLUDE_FILES ${SRC_INCLUDE_${module}_FILES})
endforeach ()

set(LOCAL_OUTPUT_DIR ${PROJECT_SOURCE_DIR})

add_library(${PROJECT_NAME}_STATIC STATIC ${INCLUDE_FILES} ${SRC_INCLUDE_FILES} ${SRC_FILES})
add_library(${PROJECT_NAME}_SHARED SHARED ${INCLUDE_FILES} ${SRC_INCLUDE_FILES} ${SRC_FILES})
# target_link_libraries(${PROJECT_NAME}_SHARED ${PROJECT_NAME}_STATIC)

if (ORZ_WITH_OPENSSL)
    message(STATUS "Build with OpenSSL")
    add_definitions(-DORZ_WITH_OPENSSL)
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME}_SHARED ${OPENSSL_CRYPTO_LIBRARY})
    target_link_libraries(${PROJECT_NAME}_STATIC ${OPENSSL_CRYPTO_LIBRARY})
    target_link_libraries(${PROJECT_NAME}_SHARED ${OPENSSL_SSL_LIBRARY})
    target_link_libraries(${PROJECT_NAME}_STATIC ${OPENSSL_SSL_LIBRARY})
endif (ORZ_WITH_OPENSSL)

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