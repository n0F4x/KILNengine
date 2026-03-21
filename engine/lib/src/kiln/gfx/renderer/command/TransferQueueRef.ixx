export module kiln.gfx.renderer.command.TransferQueueRef;

import vulkan_hpp;

import kiln.gfx.renderer.command.QueueRefBase;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.command.SubmitInfo;

namespace kiln::gfx::renderer {

export class TransferQueueRef : public QueueRefBase {
public:
    using QueueRefBase::QueueRefBase;

    auto submit(
        const TransferCommandBuffer& command_buffer,
        const SubmitInfo&            info = {}
    ) const -> void;
};

}   // namespace kiln::gfx::renderer
