module kiln.gfx.renderer.command.ComputeQueue;

namespace kiln::gfx::renderer {

auto ComputeQueue::submit(
    const ComputeCommandBuffer& command_buffer,
    const SubmitInfo&           info
) -> void
{
    QueueBase::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
