project(simple_scene)

add_executable(${PROJECT_NAME})

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

add_subdirectory(shaders)
add_subdirectory(workflow)

target_sources(${PROJECT_NAME} PRIVATE
        Camera.cxx
        Context.cxx
        Controller.cxx
        gltf_utils.cxx
        main.cpp
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        AABB.ixx
        Camera.ixx
        Context.ixx
        Controller.ixx
        gltf_utils.ixx
        SPSCQueue.ixx
)
