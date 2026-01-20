cmake_minimum_required(VERSION 4.1)

project(kiln)

set(project_prefix KILN_)

option(${project_prefix}ENABLE_TESTS "Enables building tests" OFF)
option(${project_prefix}ENABLE_EXAMPLES "Enables building examples" OFF)


# We only support MSVC for now
add_compile_options(
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
        /utf-8  # MSVC modules need this to be compatible with regular implementation units
)


add_subdirectory(lib)

if (${project_prefix}ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (${project_prefix}ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif ()
