add_subdirectory(command)
add_subdirectory(descriptor)
add_subdirectory(device)
add_subdirectory(memory)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        renderer.cppm
        Bundle.ixx
)
