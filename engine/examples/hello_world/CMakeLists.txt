project(hello_world)

include(CheckCXXSymbolExists)

add_executable(${PROJECT_NAME})

if (${MINGW})
    check_cxx_symbol_exists(__GLIBCXX__ version GLIBCXX)
    if (GLIBCXX)
        target_link_libraries(${PROJECT_NAME} PRIVATE
                stdc++exp # required for std::print on Windows with libstdc++
        )
    endif ()
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

target_sources(${PROJECT_NAME} PRIVATE
        main.cpp
)
