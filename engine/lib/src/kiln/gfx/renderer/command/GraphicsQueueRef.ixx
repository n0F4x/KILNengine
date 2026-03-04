export module kiln.gfx.renderer.command.GraphicsQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.command.TransferQueueRef;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class GraphicsQueueRef : public TransferQueueRef {
public:
    explicit GraphicsQueueRef(
        const vk::raii::Queue&         queue_ref,
        const vulkan::QueueFamilyIndex family_index
    )
        : TransferQueueRef{ queue_ref, family_index }
    {
    }
};

}   // namespace kiln::gfx::renderer
