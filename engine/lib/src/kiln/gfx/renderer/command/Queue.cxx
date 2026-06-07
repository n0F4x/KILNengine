module;

#include <utility>

module kiln.gfx.renderer.command.Queue;

import vulkan_hpp;

import kiln.gfx.vulkan.result.check_result;

namespace kiln::gfx::renderer {

Queue::Queue(
    const Device&                  device,
    const vulkan::QueueFamilyIndex family_index,
    const vk::QueueFlags           flags,
    const uint32_t                 index
)
    : m_family_index{ family_index },
      m_flags{ flags },
      m_index{ index },
      m_supports_presentation{ device.queue_family(family_index).supports_presentation() },
      m_queue{
          vulkan::check_result(
              device.logical_device().getQueue2(
                  vk::DeviceQueueInfo2{
                      .queueFamilyIndex = family_index.underlying(),
                      .queueIndex       = index,
                  }
              )   //
          ),
      }
{
}

auto Queue::family_index() const noexcept -> vulkan::QueueFamilyIndex
{
    return m_family_index;
}

auto Queue::flags() const noexcept -> vk::QueueFlags
{
    return m_flags;
}

auto Queue::index() const noexcept -> uint32_t
{
    return m_index;
}

auto Queue::supports_presentation() const noexcept -> uint32_t
{
    return m_supports_presentation;
}

auto Queue::get() noexcept -> const vk::raii::Queue&
{
    return m_queue;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Queue::submit(const CommandBufferBase& command_buffer, const SubmitInfo& info)
    -> void
{
    const vk::CommandBufferSubmitInfo command_buffer_submit_info{
        .commandBuffer = command_buffer.get()
    };

    const vk::SubmitInfo2 submit_info{
        .waitSemaphoreInfoCount = static_cast<uint32_t>(info.wait_semaphores().size()),
        .pWaitSemaphoreInfos    = info.wait_semaphores().data(),
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos    = &command_buffer_submit_info,
        .signalSemaphoreInfoCount
        = static_cast<uint32_t>(info.signal_semaphores().size()),
        .pSignalSemaphoreInfos = info.signal_semaphores().data(),
    };

    m_queue.submit2(submit_info, info.fence().value_or(vk::Fence{}));
}

}   // namespace kiln::gfx::renderer
