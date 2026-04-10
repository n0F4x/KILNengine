module;

#include <memory>

#include <vk_mem_alloc.h>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.memory.Allocator;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocation;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::renderer {

namespace internal {

export class AllocatorBuilder;

}   // namespace internal

export class Allocator {
public:
    using Builder = internal::AllocatorBuilder;


    Allocator(
        const vulkan::Instance& instance,
        [[kiln_lifetimebound]]
        const Device& device
    );


    [[nodiscard]]
    auto get() -> VmaAllocator;


    [[nodiscard]]
    auto create_buffer(
        const vk::BufferCreateInfo&    buffer_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    ) -> std::tuple<Buffer, VmaAllocationInfo>;

private:
    std::reference_wrapper<const Device>                            m_device;
    std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)> m_handle;
};

namespace internal {

export class AllocatorBuilder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto
        create(vulkan::InstanceBuilder& instance_builder, DeviceBuilder& device_builder)
            -> AllocatorBuilder;

    [[nodiscard]]
    static auto build(const vulkan::Instance& instance, const Device& device)
        -> Allocator;
};

}   // namespace internal

}   // namespace kiln::gfx::renderer
