module;

#include <utility>

#include <vk_mem_alloc.h>

module kiln.gfx.renderer.memory.RingBuffer;

import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

RingBuffer::RingBuffer(RingBuffer&& other) noexcept
    : m_allocator{ other.m_allocator },
      m_pool{ std::exchange(other.m_pool, nullptr) }
{
}

RingBuffer::~RingBuffer()
{
    if (m_pool)
    {
        vmaDestroyPool(m_allocator, m_pool);
    }
}

[[nodiscard]]
auto create_pool(Allocator& allocator, const RingBuffer::Options& options) -> VmaPool
{
    const vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = options.buffer_flags,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext = &buffer_usage_flags,
    };
    const VmaAllocationCreateInfo allocation_create_info{
        .flags = options.allocation_flags,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    uint32_t memory_type_index;
    vulkan::check_result(
        vmaFindMemoryTypeIndexForBufferInfo(
            allocator.get(),
            reinterpret_cast<const decltype(buffer_create_info)::NativeType*>(
                &buffer_create_info
            ),
            &allocation_create_info,
            &memory_type_index
        )   //
    );

    const VmaPoolCreateInfo pool_create_info{
        .memoryTypeIndex = memory_type_index,
        .blockSize       = options.size,
        .maxBlockCount   = 1,
    };

    VmaPool pool;
    vulkan::check_result(vmaCreatePool(allocator.get(), &pool_create_info, &pool));

    return pool;
}

RingBuffer::RingBuffer(Allocator& allocator, const Options& options)
    : m_allocator{ allocator.get() },
      m_pool{ create_pool(allocator, options) }
{
}

auto RingBuffer::operator=(RingBuffer&& other) noexcept -> RingBuffer&
{
    if (&other == this)
    {
        return *this;
    }

    if (m_pool)
    {
        vmaDestroyPool(m_allocator, m_pool);
    }

    std::swap(m_allocator, other.m_allocator);
    std::swap(m_pool, other.m_pool);

    return *this;
}

}   // namespace kiln::gfx::renderer
