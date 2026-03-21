module kiln.gfx.renderer.command.TransferQueueRef;

namespace kiln::gfx::renderer {

auto TransferQueueRef::submit(
    const TransferCommandBuffer& command_buffer,
    const SubmitInfo&            info
) const -> void
{
    QueueRefBase::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
