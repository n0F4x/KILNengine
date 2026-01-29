cmake_minimum_required(VERSION 4.1)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

project(kiln)

set(project_prefix KILN_)

option(${project_prefix}DEBUG "Activate debug functionalities" OFF)
option(${project_prefix}ENABLE_TESTS "Enables building tests" OFF)
option(${project_prefix}ENABLE_EXAMPLES "Enables building examples" OFF)


# We only support MSVC for now
if (${MSVC})
    add_compile_options(
            /WX
            /W4
            /w14242
            /w14254
            /w14263
            /w14265
            /w14287
            /we4289
            /w14296
            /w14311
            /w14545
            /w14546
            /w14547
            /w14549
            /w14555
            /w14619
            /w14640
            /w14826
            /w14905
            /w14906
            /w14928
            /permissive-
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 21)
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
