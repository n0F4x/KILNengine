export module kiln.gfx.renderer.command.GraphicsQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.command.TransferQueueRef;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class GraphicsQueueRef : public TransferQueueRef {
public:
    explicit GraphicsQueueRef(
        vulkan::QueueFamilyIndex family_index,
        const vk::raii::Queue&   queue_ref
    );
};

}   // namespace kiln::gfx::renderer
