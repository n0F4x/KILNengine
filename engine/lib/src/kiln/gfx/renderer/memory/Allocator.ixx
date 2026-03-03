module;

#include <memory>

#include <vk_mem_alloc.h>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.memory.Allocator;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocation;
import kiln.gfx.renderer.memory.Buffer;

namespace kiln::gfx::renderer {

export class Allocator {
public:
    Allocator(
        const vk::raii::Instance& instance,
        [[kiln_lifetimebound]]
        const Device& device
    );

    [[nodiscard]]
    auto create_buffer(
        const vk::BufferCreateInfo&    buffer_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    ) const -> std::tuple<Buffer, Allocation, VmaAllocationInfo>;

private:
    std::reference_wrapper<const Device>                            m_device;
    std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)> m_handle;
};

}   // namespace kiln::gfx::renderer
