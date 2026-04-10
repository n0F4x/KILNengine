module;

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.TransferCommandBuffer;

import vulkan_hpp;

import kiln.util.contracts;

namespace kiln::gfx::renderer {

// ReSharper disable once CppMemberFunctionMayBeConst
auto TransferCommandBuffer::enqueue_buffer_copy(
    Buffer& source,
    Buffer& destination
) -> void
{
    PRECOND(source.size() == destination.size());

    const vk::BufferCopy2 region{
        .size = source.size(),
    };

    const vk::CopyBufferInfo2 info{
        .srcBuffer   = source.get(),
        .dstBuffer   = destination.get(),
        .regionCount = 1u,
        .pRegions    = &region,
    };

    get().copyBuffer2(info);
}

}   // namespace kiln::gfx::renderer
