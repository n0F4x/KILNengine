module kiln.gfx.renderer.command.TransferQueueRef;

namespace kiln::gfx::renderer {

TransferQueueRef::TransferQueueRef(
    const vk::raii::Queue&         queue_ref,
    const vulkan::QueueFamilyIndex queue_family_index
)
    : m_queue_ref{ queue_ref },
      m_family_index{ queue_family_index }
{
}

auto TransferQueueRef::family_index() const noexcept -> vulkan::QueueFamilyIndex
{
    return m_family_index;
}

auto TransferQueueRef::submit(
    const OneTimeTransferCommandBuffer& one_time_transfer_command_buffer,
    const SubmitInfo&                   info
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

}   // namespace kiln::gfx::renderer
