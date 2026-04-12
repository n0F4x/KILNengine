export module kiln.gfx.renderer.command.TransferCommandBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferBase;
import kiln.gfx.renderer.memory.Buffer;

namespace kiln::gfx::renderer {

export class TransferCommandBuffer : public CommandBufferBase {
public:
    using CommandBufferBase::CommandBufferBase;

    auto record_buffer_copy(Buffer& source, Buffer& destination) -> void;
};

}   // namespace kiln::gfx::renderer
