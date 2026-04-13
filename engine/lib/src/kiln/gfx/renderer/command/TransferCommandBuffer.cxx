module;

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.TransferCommandBuffer;

import vulkan_hpp;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

// ReSharper disable once CppMemberFunctionMayBeConst
auto TransferCommandBuffer::record_buffer_copy(
    const BufferRegion& source,
    const BufferRegion& destination
) -> void
{
    PRECOND(source.size() == destination.size());

    const vk::BufferCopy2 region{
        .srcOffset = source.offset(),
        .dstOffset = destination.offset(),
        .size      = source.size(),
    };

    const vk::CopyBufferInfo2 info{
        .srcBuffer   = source.buffer(),
        .dstBuffer   = destination.buffer(),
        .regionCount = 1u,
        .pRegions    = &region,
    };

    get().copyBuffer2(info);
}

}   // namespace kiln::gfx::renderer
