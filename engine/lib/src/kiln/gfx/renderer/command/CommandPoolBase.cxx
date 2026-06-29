module;

#include <utility>

module kiln.gfx.renderer.command.CommandPoolBase;

import vulkan;

import kiln.gfx.renderer.command.CommandPoolFlags;
import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

CommandPoolBase::CommandPoolBase(
    const Device&                          device,
    const vulkan::QueueFamilyIndex         queue_family_index,
    const util::EnumMask<CommandPoolFlags> flags
)
    : m_device{ device },
      m_queue_family_index{ queue_family_index },
      m_command_pool{
          vulkan::check_result(
              device.logical_device().createCommandPool(
                  vk::CommandPoolCreateInfo{
                      .flags = vk::CommandPoolCreateFlagBits::eTransient
                             | vk::CommandPoolCreateFlags{ flags.underlying() },
                      .queueFamilyIndex = queue_family_index.underlying(),
                  }
              )   //
          ),
      }
{
}

auto CommandPoolBase::queue_family_index() const noexcept -> vulkan::QueueFamilyIndex
{
    return m_queue_family_index;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandPoolBase::reset() -> void
{
    m_command_pool.reset();
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
