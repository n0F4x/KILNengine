cmake_minimum_required(VERSION 4.1)

include(CheckCXXSymbolExists)

project(kiln)

set(project_prefix KILN_)

option(${project_prefix}DEBUG "Activate debug functionalities" OFF)
option(${project_prefix}ENABLE_TESTS "Enables building tests" OFF)
option(${project_prefix}ENABLE_EXAMPLES "Enables building examples" OFF)


# We only support Clang for now
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    check_cxx_symbol_exists(_LIBCPP_VERSION "vector" LIBCPP)

    add_compile_options(
            -Werror
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wno-missing-field-initializers
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wcast-qual
            -Wnrvo
            -Wimplicit-fallthrough
            -Wno-unused-command-line-argument
    )
    if (${project_prefix}DEBUG AND LIBCPP)
        add_compile_definitions(
                _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG
        )
    endif ()
endif ()


if (${project_prefix}DEBUG)
    add_compile_definitions(${project_prefix}DEBUG)
endif ()


add_subdirectory(lib)

if (${project_prefix}ENABLE_TESTS)
    target_compile_definitions(engine PUBLIC
            ${project_prefix}TEST
    )

    enable_testing()
    add_subdirectory(tests)
endif ()

if (${project_prefix}ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif ()
