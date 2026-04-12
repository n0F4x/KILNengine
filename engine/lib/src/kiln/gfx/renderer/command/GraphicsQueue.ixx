export module kiln.gfx.renderer.command.GraphicsQueue;

import vulkan_hpp;

import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.command.TransferQueue;

namespace kiln::gfx::renderer {

export class GraphicsQueue : public TransferQueue {
public:
    using TransferQueue::TransferQueue;

    auto submit(
        const GraphicsCommandBuffer& command_buffer,
        const SubmitInfo&            info = {}
    ) -> void;
};

}   // namespace kiln::gfx::renderer
