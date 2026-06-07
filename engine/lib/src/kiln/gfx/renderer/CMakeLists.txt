add_subdirectory(command)
add_subdirectory(device)
add_subdirectory(memory)
add_subdirectory(pipeline)
add_subdirectory(presentation)
add_subdirectory(stream)

target_sources(${PROJECT_NAME} PRIVATE
        Bundle.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        renderer.cppm
        Bundle.ixx
)
