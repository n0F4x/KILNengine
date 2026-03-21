module kiln.gfx.renderer.command.GraphicsQueueRef;

namespace kiln::gfx::renderer {

auto GraphicsQueueRef::submit(
    const GraphicsCommandBuffer& command_buffer,
    const SubmitInfo&            info
) const -> void
{
    QueueRefBase::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
