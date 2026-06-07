module;

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.GraphicsQueueRef;

import vulkan_hpp;

import kiln.util.contracts;

namespace kiln::gfx::renderer {

GraphicsQueueRefPrecondition::GraphicsQueueRefPrecondition(const Queue& queue)
{
    PRECOND(queue.flags() & vk::QueueFlagBits::eGraphics);
}

GraphicsQueueRef::GraphicsQueueRef(Queue& queue)
    : GraphicsQueueRefPrecondition{ queue },
      TransferQueueRef{ queue }
{
}

auto GraphicsQueueRef::submit(
    const GraphicsCommandBuffer& command_buffer,
    const SubmitInfo&            info
) const -> void
{
    TransferQueueRef::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
