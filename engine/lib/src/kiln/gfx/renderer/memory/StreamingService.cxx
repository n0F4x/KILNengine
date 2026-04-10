module;

#include <memory_resource>

#include <vk_mem_alloc.h>

module kiln.gfx.renderer.memory.StreamingService;

import vulkan_hpp;

namespace kiln::gfx::renderer {

StreamingService::StreamingService(Allocator& allocator)
    : StreamingService{ std::allocator_arg, std::pmr::get_default_resource(), allocator }
{
}

[[nodiscard]]
consteval auto staging_ring_buffer_options() -> RingBuffer::Options
{
    return RingBuffer::Options{
        .buffer_flags     = vk::BufferUsageFlagBits2::eTransferSrc,
        .allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .size             = 64ull * 1'024 * 1'024,   // 64 MiB
    };
}

StreamingService::StreamingService(
    std::allocator_arg_t,
    const allocator_type& allocator,
    Allocator&            device_allocator
)
    : m_staging_ring_buffer{ device_allocator, staging_ring_buffer_options() },
      m_memory_resource{ std::allocator_arg, allocator, allocator.resource() },
      m_staging_buffers{ &*m_memory_resource }
{
}

}   // namespace kiln::gfx::renderer
