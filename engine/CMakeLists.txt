cmake_minimum_required(VERSION 4.1)

project(kiln)

set(project_prefix KILN_)

option(${project_prefix}DEBUG "Activate debug functionalities" OFF)
option(${project_prefix}ENABLE_TESTS "Enables building tests" OFF)
option(${project_prefix}ENABLE_EXAMPLES "Enables building examples" OFF)


# We only support Clang for now
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
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
    )
    if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL 21)
        add_compile_options(
                -Wnrvo
        )
    endif ()
endif ()


if (${project_prefix}DEBUG)
    add_compile_definitions(${project_prefix}DEBUG)
endif ()


add_subdirectory(lib)

if (${project_prefix}ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (${project_prefix}ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif ()
