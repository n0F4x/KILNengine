cmake_minimum_required(VERSION 4.1)

project(hello_triangle)

add_executable(${PROJECT_NAME})

add_subdirectory(shaders)

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        demo.ixx
)

target_sources(${PROJECT_NAME} PRIVATE
        demo.cxx
        main.cpp
)
