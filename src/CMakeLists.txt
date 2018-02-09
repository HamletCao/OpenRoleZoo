project(ORZ)

set(ORZ_SOURCE_DIR ${PROJECT_SOURCE_DIR}/..)

option(ORZ_WITH_OPENSSL "Supporting AES, RAS and SSL with OpenSSL" OFF)

include_directories(${ORZ_SOURCE_DIR}/include)
include_directories(${ORZ_SOURCE_DIR}/src)

set(ORZ_MODULES codec io lego mem net sync tools utils)

if (ORZ_WITH_OPENSSL)
    LIST(APPEND ORZ_MODULES ssl)
endif (ORZ_WITH_OPENSSL)

set(ORZ_INCLUDE_FILES)
set(ORZ_SRC_FILES)
set(ORZ_SRC_INCLUDE_FILES)

foreach (module ${ORZ_MODULES})
    FILE(GLOB_RECURSE ORZ_INCLUDE_${module}_FILES ${ORZ_SOURCE_DIR}/include/orz/${module}/*.h)
    LIST(APPEND ORZ_INCLUDE_FILES ${ORZ_INCLUDE_${module}_FILES})

    FILE(GLOB_RECURSE ORZ_SRC_${module}_FILES ${ORZ_SOURCE_DIR}/src/orz/${module}/*.cpp)
    LIST(APPEND ORZ_SRC_FILES ${ORZ_SRC_${module}_FILES})

    FILE(GLOB_RECURSE ORZ_SRC_INCLUDE_${module}_FILES ${ORZ_SOURCE_DIR}/src/orz/${module}/*.h)
    LIST(APPEND ORZ_SRC_INCLUDE_FILES ${ORZ_SRC_INCLUDE_${module}_FILES})
endforeach ()


add_library(${PROJECT_NAME}_STATIC STATIC ${ORZ_INCLUDE_FILES} ${ORZ_SRC_INCLUDE_FILES} ${ORZ_SRC_FILES})
add_library(${PROJECT_NAME}_SHARED SHARED ${ORZ_INCLUDE_FILES} ${ORZ_SRC_INCLUDE_FILES} ${ORZ_SRC_FILES})
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

if (ORZ_INSTALL)
    install(TARGETS ${PROJECT_NAME}_SHARED DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    install(TARGETS ${PROJECT_NAME}_STATIC DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    install(FILES ${INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/orz)

    if (WIN32)
        install(FILES ${LOCAL_OUTPUT_DIR}/lib/lib${PROJECT_NAME}.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif ()
endif (ORZ_INSTALL)
