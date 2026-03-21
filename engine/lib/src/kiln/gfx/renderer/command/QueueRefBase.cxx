module;

#include <cstdint>

module kiln.gfx.renderer.command.QueueRefBase;

namespace kiln::gfx::renderer {

QueueRefBase::QueueRefBase(
    const vulkan::QueueFamilyIndex queue_family_index,
    const vk::raii::Queue&         queue_ref
)
    : m_family_index{ queue_family_index },
      m_queue_ref{ queue_ref }
{
}

auto QueueRefBase::family_index() const noexcept -> vulkan::QueueFamilyIndex
{
    return m_family_index;
}

auto QueueRefBase::get() const noexcept -> const vk::raii::Queue&
{
    return m_queue_ref;
}

auto QueueRefBase::submit(
    const CommandBufferBase& command_buffer,
    const SubmitInfo&        info
) const -> void
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

    m_queue_ref.get().submit2(
        submit_info,
        info.fence.transform(&vk::raii::Fence::operator*).value_or(vk::Fence{})
    );
}

}   // namespace kiln::gfx::renderer
