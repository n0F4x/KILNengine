project(hello_app)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

target_sources(${PROJECT_NAME} PRIVATE
        main.cpp
)
