module kiln.gfx.renderer.device.GraphicsQueueRef;

namespace kiln::gfx::renderer {

GraphicsQueueRef::GraphicsQueueRef(
    const vk::raii::Queue&         queue_ref,
    const vulkan::QueueFamilyIndex family_index
)
    : TransferQueueRef{ queue_ref, family_index }
{
}

}   // namespace kiln::gfx::renderer
