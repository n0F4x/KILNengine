export module kiln.gfx.renderer.command.ComputeQueue;

import vulkan_hpp;

import kiln.gfx.renderer.command.ComputeCommandBuffer;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.command.TransferQueue;

namespace kiln::gfx::renderer {

export class ComputeQueue : public TransferQueue {
public:
    using TransferQueue::TransferQueue;

    auto submit(
        const ComputeCommandBuffer& command_buffer,
        const SubmitInfo&           info = SubmitInfo{}
    ) -> void;
};

}   // namespace kiln::gfx::renderer
