cmake_minimum_required(VERSION 4.1)

project(simple_scene)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

add_subdirectory(src)
