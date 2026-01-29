if (DEFINED CMAKE_TOOLCHAIN_FILE)
    # This message is here to mitigate the "CMAKE_TOOLCHAIN_FILE not used warning"
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif ()


# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
