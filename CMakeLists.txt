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

# dependencies
if (NOT DISABLE_EDM)
    evc_setup_edm()
    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(EVISO15118_INSTALL OFF)
else()
    find_package(fsm REQUIRED)
    find_package(cbv2g REQUIRED)
endif()

add_subdirectory(input)
add_subdirectory(src)

if (EVISO15118_BUILD_TESTS)
    include(CTest)
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
