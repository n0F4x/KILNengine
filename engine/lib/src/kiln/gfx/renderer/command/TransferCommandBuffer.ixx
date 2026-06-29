export module kiln.gfx.renderer.command.TransferCommandBuffer;

import vulkan;

import kiln.gfx.renderer.command.CommandBufferBase;
import kiln.gfx.renderer.memory.BufferRegion;

namespace kiln::gfx::renderer {

export class TransferCommandBuffer : public CommandBufferBase {
public:
    using CommandBufferBase::CommandBufferBase;

    auto record_buffer_copy(const BufferRegion& source, const BufferRegion& destination)
        -> void;
};

}   // namespace kiln::gfx::renderer
