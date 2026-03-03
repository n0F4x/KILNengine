module;

#include <span>

#include <vk_mem_alloc.h>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.memory.Allocation;

import vulkan_hpp;

import kiln.gfx.renderer.memory.MemoryTypeID;
import kiln.gfx.renderer.memory.MemoryView;

namespace kiln::gfx::renderer {

export class Allocation {
public:
    explicit Allocation(
        VmaAllocator   allocator,
        VmaAllocation  allocation,
        MemoryTypeID   memory_type_id,
        vk::DeviceSize size
    ) noexcept;
    Allocation(const Allocation&) = delete;
    Allocation(Allocation&&) noexcept;
    ~Allocation() noexcept;

    auto operator=(const Allocation&) -> Allocation& = delete;
    auto operator=(Allocation&&) noexcept -> Allocation&;

    [[nodiscard]]
    auto memory_type_id() const noexcept -> MemoryTypeID;
    [[nodiscard]]
    auto memory_view() const noexcept [[kiln_lifetimebound]] -> MemoryView;
    [[nodiscard]]
    auto memory_properties() const noexcept -> vk::MemoryPropertyFlags;

    auto reset() noexcept -> void;

    [[nodiscard]]
    auto map() const noexcept -> std::span<std::byte>;
    auto unmap() const noexcept -> void;

    auto invalidate(vk::DeviceSize offset, vk::DeviceSize size) const -> void;
    auto flush(vk::DeviceSize offset, vk::DeviceSize size) const -> void;

protected:
    [[nodiscard]]
    auto allocation() const noexcept -> VmaAllocation;
    [[nodiscard]]
    auto allocator() const noexcept -> VmaAllocator;

private:
    VmaAllocator  m_allocator;
    VmaAllocation m_allocation;

    MemoryTypeID   m_memory_type_id;
    vk::DeviceSize m_size;
};

}   // namespace kiln::gfx::renderer
