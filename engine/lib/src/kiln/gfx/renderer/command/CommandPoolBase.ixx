module;

#include <functional>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.command.CommandPoolBase;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandPoolFlags;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.util.EnumMask;

namespace kiln::gfx::renderer {

export class CommandPoolBase {
public:
    CommandPoolBase(
        [[kiln_lifetimebound]]
        const Device&                    device,
        vulkan::QueueFamilyIndex         queue_family_index,
        util::EnumMask<CommandPoolFlags> flags = CommandPoolFlags::eNone
    );

protected:
    auto allocate_primary() -> vk::raii::CommandBuffer;

private:
    std::reference_wrapper<const Device> m_device;
    vk::raii::CommandPool                m_command_pool;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

CommandPoolBase::CommandPoolBase(
    const Device&                          device,
    const vulkan::QueueFamilyIndex         queue_family_index,
    const util::EnumMask<CommandPoolFlags> flags
)
    : m_device{ device },
      m_command_pool{
          vulkan::check_result(device.logical_device().createCommandPool(
              vk::CommandPoolCreateInfo{
                  .flags = vk::CommandPoolCreateFlagBits::eTransient
                         | vk::CommandPoolCreateFlags{ flags.underlying() },
                  .queueFamilyIndex = queue_family_index.underlying(),
              }
          ))   //
      }
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandPoolBase::allocate_primary() -> vk::raii::CommandBuffer
{
    const vk::CommandBufferAllocateInfo allocate_info{
        .commandPool        = m_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    return std::move(
        vulkan::check_result(
            m_device.get().logical_device().allocateCommandBuffers(allocate_info)
        )
            .front()
    );
}

}   // namespace kiln::gfx::renderer
