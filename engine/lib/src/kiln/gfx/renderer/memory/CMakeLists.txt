target_sources(${PROJECT_NAME} PRIVATE
        Allocation.cxx
        Allocator.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        memory.cppm
        Allocation.ixx
        Allocator.ixx
        AllocatorBuilder.ixx
        Buffer.ixx
        MemoryTypeID.ixx
        MemoryView.ixx
)
