cmake_minimum_required(VERSION 4.1)

project(engine VERSION 0.0.0)


add_library(${PROJECT_NAME})
add_library(kiln::engine ALIAS ${PROJECT_NAME})


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)


include(cmake/dependencies.cmake)


add_subdirectory(src)


install(TARGETS ${PROJECT_NAME}
        EXPORT kilnTargets
        FILE_SET public_headers FILE_SET public_cxx_modules
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
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
