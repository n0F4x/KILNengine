module;

#include <cstddef>
#include <memory>
#include <optional>
#include <span>

#include <vk_mem_alloc.h>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.memory.Allocator;

import vulkan_hpp;

import kiln.app.context.ContextBase;
import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.memory.Allocation;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.memory.Image;
import kiln.gfx.renderer.memory.LazyCopy;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::renderer {

export class Allocator : public app::ContextBase {
public:
    class Builder;


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
        const VmaAllocationCreateInfo& allocation_create_info,
        std::optional<uint32_t>        min_alignment = std::nullopt
    ) -> Buffer;
    [[nodiscard]]
    auto create_image(
        const vk::ImageCreateInfo&     image_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    ) -> Image;

    auto host_copy(
        std::span<const std::byte> source,
        Allocation&                destination,
        vk::DeviceSize             destination_offset,
        vk::DeviceSize             destination_size
    ) -> void;
    auto host_copy(std::span<const std::byte> source, const BufferRegion& destination)
        -> void;
    auto host_copy(const LazyCopy& lazy_copy, const BufferRegion& destination) -> void;

    [[nodiscard]]
    auto map(Allocation& allocation) -> std::span<std::byte>;
    auto map(const BufferRegion& buffer_region) -> std::span<std::byte>;
    auto unmap(Allocation& allocation) -> void;
    auto unmap(const BufferRegion& buffer_region) -> void;

    auto invalidate(
        Allocation&    allocation,
        vk::DeviceSize destination_offset,
        vk::DeviceSize destination_size
    ) -> void;
    auto invalidate(const BufferRegion& buffer) -> void;
    auto try_invalidate(
        Allocation&    allocation,
        vk::DeviceSize destination_offset,
        vk::DeviceSize destination_size
    ) -> bool;
    auto try_invalidate(const BufferRegion& buffer) -> bool;
    auto flush(
        Allocation&    allocation,
        vk::DeviceSize destination_offset,
        vk::DeviceSize destination_size
    ) -> void;
    auto flush(const BufferRegion& buffer) -> void;
    auto try_flush(
        Allocation&    allocation,
        vk::DeviceSize destination_offset,
        vk::DeviceSize destination_size
    ) -> bool;
    auto try_flush(const BufferRegion& buffer) -> bool;

private:
    std::reference_wrapper<const Device>                            m_device;
    std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)> m_handle;
};

class Allocator::Builder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto create(
        vulkan::InstanceBuilder& instance_builder,
        DeviceBuilder&           device_builder
    ) -> Builder;

    [[nodiscard]]
    static auto build(const vulkan::Instance& instance, const Device& device)
        -> Allocator;
};

}   // namespace kiln::gfx::renderer
