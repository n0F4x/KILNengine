add_subdirectory(type_traits)

target_sources(${PROJECT_NAME} PRIVATE
        Any.cpp
        OptionalRef.cpp
)
