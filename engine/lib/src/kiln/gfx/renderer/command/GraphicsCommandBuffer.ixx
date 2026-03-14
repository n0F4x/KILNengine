module;

#include <vector>

export module kiln.gfx.renderer.command.GraphicsCommandBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.util.Strong;

namespace kiln::gfx::renderer {

export class GraphicsCommandBuffer : public TransferCommandBuffer {
public:
    explicit GraphicsCommandBuffer(vk::raii::CommandBuffer&& command_buffer);
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

GraphicsCommandBuffer::GraphicsCommandBuffer(vk::raii::CommandBuffer&& command_buffer)
    : TransferCommandBuffer{ std::move(command_buffer) }
{
}

}   // namespace kiln::gfx::renderer
