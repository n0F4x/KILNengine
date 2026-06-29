module;

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.ComputeQueueRef;

import vulkan;

import kiln.util.contracts;

namespace kiln::gfx::renderer {

ComputeQueueRefPrecondition::ComputeQueueRefPrecondition(
    [[maybe_unused]] const Queue& queue
)
{
    PRECOND(queue.flags() & vk::QueueFlagBits::eCompute);
}

ComputeQueueRef::ComputeQueueRef(Queue& queue)
    : ComputeQueueRefPrecondition{ queue },
      TransferQueueRef{ queue }
{
}

auto ComputeQueueRef::submit(
    const ComputeCommandBuffer& command_buffer,
    const SubmitInfo&           info
) const -> void
{
    TransferQueueRef::submit(command_buffer, info);
}

}   // namespace kiln::gfx::renderer
