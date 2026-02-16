module;

#include <cstdint>
#include <deque>

export module kiln.gfx.renderer.CommandPool;

import vulkan_hpp;

import kiln.gfx.renderer.CommandBufferHandle;
import kiln.gfx.renderer.Device;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class CommandPool {
public:
    CommandPool(
        const vk::raii::Device&  device,
        vulkan::QueueFamilyIndex graphics_queue_family_index
    );

    auto allocate(const vk::raii::Device& device) -> CommandBufferHandle;
    auto reset() -> void;

private:
    vk::raii::CommandPool               m_pool;
    std::deque<vk::raii::CommandBuffer> m_buffers;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

namespace internal {

[[nodiscard]]
auto create_command_pool(
    const vk::raii::Device&        device,
    const vulkan::QueueFamilyIndex graphics_queue_family_index
) -> vk::raii::CommandPool
{
    const vk::CommandPoolCreateInfo create_info{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = graphics_queue_family_index.underlying()
    };

    return vulkan::check_result(device.createCommandPool(create_info));
}

}   // namespace internal

CommandPool::CommandPool(
    const vk::raii::Device&        device,
    const vulkan::QueueFamilyIndex graphics_queue_family_index
)
    : m_pool{ internal::create_command_pool(device, graphics_queue_family_index) }
{
}

auto CommandPool::allocate(const vk::raii::Device& device) -> CommandBufferHandle
{
    const vk::CommandBufferAllocateInfo allocate_info{
        .commandPool        = m_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    m_buffers.push_back(
        std::move(
            vulkan::check_result(device.allocateCommandBuffers(allocate_info)).front()
        )
    );

    return CommandBufferHandle{ static_cast<uint32_t>(m_buffers.size() - 1) };
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandPool::reset() -> void
{
    m_pool.reset();
}

}   // namespace kiln::gfx::renderer
