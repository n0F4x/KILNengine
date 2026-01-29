project(hello_world)

add_executable(${PROJECT_NAME})

if (${MINGW})
    target_link_libraries(${PROJECT_NAME} PRIVATE
            stdc++exp # required for std::print on Windows with libstdc++
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

target_sources(${PROJECT_NAME} PRIVATE
        main.cpp
)
