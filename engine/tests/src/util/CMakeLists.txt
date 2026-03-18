add_subdirectory(containers)
add_subdirectory(type_traits)

target_sources(${PROJECT_NAME} PRIVATE
        Overloaded.cpp
)
