module;

#include <utility>

module kiln.gfx.renderer.command.QueueBase;

namespace kiln::gfx::renderer {

QueueBase::QueueBase(
    const QueueType                type,
    const vulkan::QueueFamilyIndex family_index,
    const vk::QueueFlags           flags,
    const uint32_t                 index,
    vk::raii::Queue&&              queue
)
    : m_type{ type },
      m_family_index{ family_index },
      m_flags{ flags },
      m_index{ index },
      m_queue{ std::move(queue) }
{
}

auto QueueBase::type() const noexcept -> QueueType
{
    return m_type;
}

auto QueueBase::family_index() const noexcept -> vulkan::QueueFamilyIndex
{
    return m_family_index;
}

auto QueueBase::flags() const noexcept -> vk::QueueFlags
{
    return m_flags;
}

auto QueueBase::index() const noexcept -> uint32_t
{
    return m_index;
}

auto QueueBase::get() noexcept -> const vk::raii::Queue&
{
    return m_queue;
}

auto QueueBase::submit(const CommandBufferBase& command_buffer, const SubmitInfo& info)
    -> void
{
    const vk::CommandBufferSubmitInfo command_buffer_submit_info{
        .commandBuffer = command_buffer.get()
    };

    const vk::SubmitInfo2 submit_info{
        .waitSemaphoreInfoCount   = static_cast<uint32_t>(info.wait_semaphores.size()),
        .pWaitSemaphoreInfos      = info.wait_semaphores.data(),
        .commandBufferInfoCount   = 1,
        .pCommandBufferInfos      = &command_buffer_submit_info,
        .signalSemaphoreInfoCount = static_cast<uint32_t>(info.signal_semaphores.size()),
        .pSignalSemaphoreInfos    = info.signal_semaphores.data(),
    };

    m_queue.submit2(
        submit_info,
        info.fence.transform(&vk::raii::Fence::operator*).value_or(vk::Fence{})
    );
}

}   // namespace kiln::gfx::renderer
