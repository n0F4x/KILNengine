project(frustum_culling)

add_executable(${PROJECT_NAME})

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

target_link_libraries(${PROJECT_NAME} PRIVATE kiln::engine)

add_subdirectory(shaders)
add_subdirectory(workflow)

target_sources(${PROJECT_NAME} PRIVATE
        algorithms.cxx
        Camera.cxx
        Context.cxx
        Controller.cxx
        gltf_utils.cxx
        load_scene.cxx
        main.cpp
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        AABB.ixx
        algorithms.ixx
        Camera.ixx
        Context.ixx
        Controller.ixx
        gltf_utils.ixx
        load_scene.ixx
        SPSCQueue.ixx
)
