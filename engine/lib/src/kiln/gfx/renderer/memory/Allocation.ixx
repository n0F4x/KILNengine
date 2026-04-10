module;

#include <span>

#include <vk_mem_alloc.h>

export module kiln.gfx.renderer.memory.Allocation;

import vulkan_hpp;

import kiln.gfx.renderer.memory.MemoryTypeID;

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
    auto get() noexcept -> VmaAllocation;
    [[nodiscard]]
    auto allocator() noexcept -> VmaAllocator;
    [[nodiscard]]
    auto memory_type_id() const noexcept -> MemoryTypeID;
    [[nodiscard]]
    auto memory_properties() const noexcept -> vk::MemoryPropertyFlags;
    [[nodiscard]]
    auto size() const noexcept -> vk::DeviceSize;

    auto reset() noexcept -> void;

    [[nodiscard]]
    auto map() -> std::span<std::byte>;
    auto unmap() -> void;

    auto invalidate(vk::DeviceSize offset, vk::DeviceSize size) -> void;
    auto flush(vk::DeviceSize offset, vk::DeviceSize size) const -> void;

private:
    VmaAllocator  m_allocator;
    VmaAllocation m_allocation;

    MemoryTypeID   m_memory_type_id;
    vk::DeviceSize m_size;
};

}   // namespace kiln::gfx::renderer
