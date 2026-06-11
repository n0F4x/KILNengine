add_subdirectory(app)
add_subdirectory(gfx)
add_subdirectory(util)

target_sources(${PROJECT_NAME} PRIVATE
        test_test.cpp
)
