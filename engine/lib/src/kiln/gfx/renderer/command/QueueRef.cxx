module kiln.gfx.renderer.command.QueueRef;

import kiln.util.EnumMask;

namespace kiln::gfx::renderer {

QueueRef::QueueRef(Queue& queue) : QueueRefBase{ queue } {}

auto QueueRef::as_compute_queue() const -> ComputeQueueRef
{
    return ComputeQueueRef{ get() };
}

auto QueueRef::as_graphics_queue() const -> GraphicsQueueRef
{
    return GraphicsQueueRef{ get() };
}

auto QueueRef::as_present_queue() const -> PresentQueueRef
{
    return PresentQueueRef{ get() };
}

auto QueueRef::as_transfer_queue() const -> TransferQueueRef
{
    return TransferQueueRef{ get() };
}

auto QueueRef::submit(
    const CommandBufferBase& command_buffer,
    const SubmitInfo&        info
) const -> void
{
    QueueRefBase::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
