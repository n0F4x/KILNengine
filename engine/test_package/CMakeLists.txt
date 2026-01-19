cmake_minimum_required(VERSION 4.1)
project(test_package)

find_package(kiln CONFIG REQUIRED)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
