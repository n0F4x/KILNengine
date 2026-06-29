module;

#include <vk_mem_alloc.h>

export module kiln.gfx.renderer.memory.Allocation;

import vulkan;

import kiln.gfx.renderer.memory.MemoryTypeID;

namespace kiln::gfx::renderer {

export class Allocation {
public:
    Allocation(const Allocation&) = delete;
    Allocation(Allocation&&) noexcept;
    ~Allocation() noexcept;

    explicit Allocation() = default;
    explicit Allocation(
        VmaAllocator   allocator,
        VmaAllocation  allocation,
        MemoryTypeID   memory_type_id,
        vk::DeviceSize size
    ) noexcept;


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

private:
    VmaAllocator  m_allocator{};
    VmaAllocation m_allocation{};

    MemoryTypeID   m_memory_type_id{};
    vk::DeviceSize m_size{};

    auto reset() noexcept -> void;
};

}   // namespace kiln::gfx::renderer
