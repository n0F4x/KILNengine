module kiln.gfx.renderer.command.GraphicsQueue;

namespace kiln::gfx::renderer {

auto GraphicsQueue::submit(
    const GraphicsCommandBuffer& command_buffer,
    const SubmitInfo&            info
) -> void
{
    QueueBase::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
