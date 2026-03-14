module;

#include <cassert>
#include <span>
#include <utility>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

module kiln.gfx.renderer.memory.Allocation;

import kiln.gfx.renderer.memory.MemoryTypeID;

namespace kiln::gfx::renderer {

Allocation::Allocation(
    const VmaAllocator   allocator,
    const VmaAllocation  allocation,
    const MemoryTypeID   memory_type_id,
    const vk::DeviceSize size
) noexcept
    : m_allocator{ allocator },
      m_allocation{ allocation },
      m_memory_type_id{ memory_type_id },
      m_size{ size }
{
}

Allocation::Allocation(Allocation&& other) noexcept
    : Allocation{
          std::exchange(other.m_allocator, nullptr),
          std::exchange(other.m_allocation, nullptr),
          std::exchange(other.m_memory_type_id, MemoryTypeID::invalid_value()),
          std::exchange(other.m_size, 0),
      }
{
}

Allocation::~Allocation() noexcept
{
    reset();
}

auto Allocation::operator=(Allocation&& other) noexcept -> Allocation&
{
    if (this != &other)
    {
        reset();

        std::swap(m_allocator, other.m_allocator);
        std::swap(m_allocation, other.m_allocation);
        std::swap(m_memory_type_id, other.m_memory_type_id);
        std::swap(m_size, other.m_size);
    }

    return *this;
}

auto Allocation::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

auto Allocation::allocator() const noexcept -> VmaAllocator
{
    return m_allocator;
}

auto Allocation::reset() noexcept -> void
{
    if (m_allocation != nullptr)
    {
        assert(m_allocator != nullptr);
        vmaFreeMemory(m_allocator, m_allocation);
    }

    m_size           = 0;
    m_memory_type_id = MemoryTypeID::invalid_value();
    m_allocation     = nullptr;
    m_allocator      = nullptr;
}

auto Allocation::memory_type_id() const noexcept -> MemoryTypeID
{
    return m_memory_type_id;
}

auto Allocation::memory_view() const noexcept -> MemoryView
{
    VmaAllocationInfo info;
    vmaGetAllocationInfo(m_allocator, m_allocation, &info);

    return MemoryView{
        .memory = info.deviceMemory,
        .offset = info.offset,
        .size   = info.size,
    };
}

auto Allocation::memory_properties() const noexcept -> vk::MemoryPropertyFlags
{
    VkMemoryPropertyFlags result;
    vmaGetMemoryTypeProperties(m_allocator, m_memory_type_id.underlying(), &result);
    return vk::MemoryPropertyFlags{ result };
}

auto Allocation::map() const noexcept -> std::span<std::byte>
{
    assert(memory_properties() & vk::MemoryPropertyFlagBits::eHostVisible);

    void* mapped_data{};
    vmaMapMemory(m_allocator, m_allocation, &mapped_data);

    return std::span{ static_cast<std::byte*>(mapped_data), m_size };
}

auto Allocation::unmap() const noexcept -> void
{
    vmaUnmapMemory(m_allocator, m_allocation);
}

auto Allocation::invalidate(const vk::DeviceSize offset, const vk::DeviceSize size) const
    -> void
{
    vk::detail::resultCheck(
        static_cast<vk::Result>(
            vmaInvalidateAllocation(m_allocator, m_allocation, offset, size)
        ),
        "vmaInvalidateAllocation failed"
    );
}

auto Allocation::flush(const vk::DeviceSize offset, const vk::DeviceSize size) const
    -> void
{
    vk::detail::resultCheck(
        static_cast<vk::Result>(
            vmaFlushAllocation(m_allocator, m_allocation, offset, size)
        ),
        "vmaFlushAllocation failed"
    );
}

}   // namespace kiln::gfx::renderer
