add_subdirectory(app)
add_subdirectory(asset)
add_subdirectory(config)
add_subdirectory(gfx)
add_subdirectory(util)
add_subdirectory(wsi)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        kiln.cppm
)
