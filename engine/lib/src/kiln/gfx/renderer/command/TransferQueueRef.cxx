module kiln.gfx.renderer.command.TransferQueueRef;

namespace kiln::gfx::renderer {

TransferQueueRef::TransferQueueRef(
    const vulkan::QueueFamilyIndex queue_family_index,
    const vk::raii::Queue&         queue_ref
)
    : m_family_index{ queue_family_index },
      m_queue_ref{ queue_ref }
{
}

auto TransferQueueRef::family_index() const noexcept -> vulkan::QueueFamilyIndex
{
    return m_family_index;
}

auto TransferQueueRef::submit(
    const TransferCommandBuffer& one_time_transfer_command_buffer,
    const SubmitInfo&            info
) const -> void
{
    const vk::CommandBufferSubmitInfo command_buffer_submit_info{
        .commandBuffer = one_time_transfer_command_buffer.get()
    };

    const vk::SubmitInfo2 submit_info{
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos    = &command_buffer_submit_info,
    };

    m_queue_ref.get().submit2(submit_info, *info.fence);
}

auto TransferQueueRef::get() const noexcept -> const vk::raii::Queue&
{
    return m_queue_ref;
}

}   // namespace kiln::gfx::renderer
