module kiln.gfx.renderer.command.TransferQueue;

namespace kiln::gfx::renderer {

auto TransferQueue::submit(
    const TransferCommandBuffer& command_buffer,
    const SubmitInfo&            info
) -> void
{
    QueueBase::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
