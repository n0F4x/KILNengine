module;

#include <cassert>
#include <utility>

#include <vk_mem_alloc.h>

module kiln.gfx.renderer.memory.Allocation;

import vulkan_hpp;

import kiln.gfx.renderer.memory.MemoryTypeID;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

Allocation::Allocation(
    const VmaAllocator   allocator,    // NOLINT(*-misplaced-const)
    const VmaAllocation  allocation,   // NOLINT(*-misplaced-const)
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

// ReSharper disable once CppMemberFunctionMayBeConst
auto Allocation::get() noexcept -> VmaAllocation
{
    return m_allocation;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Allocation::allocator() noexcept -> VmaAllocator
{
    return m_allocator;
}

auto Allocation::memory_type_id() const noexcept -> MemoryTypeID
{
    return m_memory_type_id;
}

auto Allocation::memory_properties() const noexcept -> vk::MemoryPropertyFlags
{
    VkMemoryPropertyFlags result{};
    if (m_allocation != nullptr)
    {
        assert(m_memory_type_id != MemoryTypeID::invalid_value());
        vmaGetMemoryTypeProperties(m_allocator, m_memory_type_id.underlying(), &result);
    }

    return vk::MemoryPropertyFlags{ result };
}

auto Allocation::size() const noexcept -> vk::DeviceSize
{
    return m_size;
}

auto Allocation::reset() noexcept -> void
{
    if (m_allocation != nullptr)
    {
        vmaFreeMemory(m_allocator, m_allocation);
    }

    m_size           = 0;
    m_memory_type_id = MemoryTypeID::invalid_value();
    m_allocation     = nullptr;
    m_allocator      = nullptr;
}

}   // namespace kiln::gfx::renderer
