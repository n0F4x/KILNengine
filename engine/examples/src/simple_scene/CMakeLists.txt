cmake_minimum_required(VERSION 4.1)

project(simple_scene)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

add_subdirectory(workflow)

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
