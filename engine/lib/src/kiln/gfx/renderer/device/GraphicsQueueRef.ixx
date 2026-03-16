export module kiln.gfx.renderer.device.GraphicsQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.device.TransferQueueRef;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class GraphicsQueueRef : public TransferQueueRef {
public:
    explicit GraphicsQueueRef(
        const vk::raii::Queue&         queue_ref,
        vulkan::QueueFamilyIndex family_index
    );
};

}   // namespace kiln::gfx::renderer
