module;

#include <vector>

export module kiln.gfx.renderer.command.GraphicsCommandBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferUsageFlags;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.util.EnumMask;
import kiln.util.Strong;

namespace kiln::gfx::renderer {

export class GraphicsCommandBuffer : public TransferCommandBuffer {
public:
    GraphicsCommandBuffer(
        vk::raii::CommandBuffer&&               command_buffer,
        util::EnumMask<CommandBufferUsageFlags> usage_flags
    );
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

GraphicsCommandBuffer::GraphicsCommandBuffer(
    vk::raii::CommandBuffer&&                     command_buffer,
    const util::EnumMask<CommandBufferUsageFlags> usage_flags
)
    : TransferCommandBuffer{ std::move(command_buffer), usage_flags }
{
}

}   // namespace kiln::gfx::renderer
