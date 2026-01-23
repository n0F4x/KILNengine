cmake_minimum_required(VERSION 4.1)

project(engine VERSION 0.0.0)

set(project_prefix KILN_)


add_library(${PROJECT_NAME})
add_library(kiln::engine ALIAS ${PROJECT_NAME})


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

if (${MSVC})
    target_compile_options(${PROJECT_NAME} PUBLIC
            /Zc:preprocessor
    )
endif ()

target_compile_definitions(${PROJECT_NAME} PUBLIC
        ${project_prefix}VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        ${project_prefix}VERSION_MINOR=${PROJECT_VERSION_MINOR}
        ${project_prefix}VERSION_PATCH=${PROJECT_VERSION_PATCH}
)


include(cmake/dependencies.cmake)


add_subdirectory(src)


install(TARGETS ${PROJECT_NAME}
        EXPORT kilnTargets
        FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT kilnTargets
        FILE kilnTargets.cmake
        NAMESPACE kiln::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kiln
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/kilnConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kiln
)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/kilnConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kiln
)
