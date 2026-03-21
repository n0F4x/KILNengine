export module kiln.gfx.renderer.command.GraphicsQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.command.TransferQueueRef;

namespace kiln::gfx::renderer {

export class GraphicsQueueRef : public TransferQueueRef {
public:
    using TransferQueueRef::TransferQueueRef;

    auto submit(
        const GraphicsCommandBuffer& command_buffer,
        const SubmitInfo&            info = {}
    ) const -> void;
};

}   // namespace kiln::gfx::renderer
