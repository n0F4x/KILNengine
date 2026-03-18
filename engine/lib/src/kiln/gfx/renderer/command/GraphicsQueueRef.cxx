module kiln.gfx.renderer.command.GraphicsQueueRef;

namespace kiln::gfx::renderer {

GraphicsQueueRef::GraphicsQueueRef(
    const vulkan::QueueFamilyIndex family_index,
    const vk::raii::Queue&         queue_ref
)
    : TransferQueueRef{ family_index, queue_ref }
{
}

}   // namespace kiln::gfx::renderer
