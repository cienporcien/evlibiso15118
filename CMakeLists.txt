cmake_minimum_required(VERSION 3.14)

project(eviso15118
    VERSION 0.1
    DESCRIPTION "eviso15118 library suite"
	LANGUAGES CXX C
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(OPT_AUTODOWNLOAD_ISO20_SCHEMAS "\
Automatically download ISO15118-20 schemas.  Note: by setting this option to \
true and hence downloading the schema files, YOU accept the ISO Customer \
Licence Agreement (“Licence Agreement”), clauses 1. ISO’s Copyright, \
7. Termination, 8. Limitations, and 9. Governing Law." OFF)


option(EVISO15118_INSTALL "Enable install target" ${EVC_MAIN_PROJECT})

option(EVISO15118_LINK_CUSTOM_MBEDTLS "\
Link against a pinned mbedtls library\
" ON)

option(USE_OPENSSL "Use OpenSSL" ON)

if (USE_OPENSSL)
    find_package(OpenSSL 3 REQUIRED)
endif()

# list of compile options
set(EVISO15118_COMPILE_OPTIONS_WARNING "-Wall;-Wextra;-Wno-unused-function;-Werror" CACHE STRING "A list of compile options used")
message(STATUS "Building evlibiso15118 with the following compile options: ${EVISO15118_COMPILE_OPTIONS_WARNING}")

if((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
    set(ISO15118_BUILD_TESTING ON)
endif()


# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()
    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(EVISO15118_INSTALL OFF)
else()
    find_package(fsm REQUIRED)
    find_package(cbv2g REQUIRED)
endif()

add_subdirectory(3rd_party)
add_subdirectory(input)
add_subdirectory(src)

if (EVISO15118_BUILD_TESTS)
    include(CTest)
    list(APPEND CMAKE_MODULE_PATH ${CPM_PACKAGE_catch2_SOURCE_DIR}/extras)
    add_subdirectory(test)
endif()

if (EVISO15118_INSTALL)
    install(
        TARGETS
            eviso15118
        EXPORT eviso15118-targets
    )

    install(
        DIRECTORY include/
        TYPE INCLUDE
        PATTERN "detail" EXCLUDE
    )

    evc_setup_package(
        NAME eviso15118
        EXPORT eviso15118-targets
        NAMESPACE eviso15118
    )
endif()
