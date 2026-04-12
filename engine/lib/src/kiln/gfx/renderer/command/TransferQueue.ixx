export module kiln.gfx.renderer.command.TransferQueue;

import vulkan_hpp;

import kiln.gfx.renderer.command.QueueBase;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.command.SubmitInfo;

namespace kiln::gfx::renderer {

export class TransferQueue : public QueueBase {
public:
    using QueueBase::QueueBase;

    auto submit(
        const TransferCommandBuffer& command_buffer,
        const SubmitInfo&            info = {}
    ) -> void;
};

}   // namespace kiln::gfx::renderer
