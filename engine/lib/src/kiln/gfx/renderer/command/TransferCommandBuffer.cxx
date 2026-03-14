module;

#include <utility>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.TransferCommandBuffer;

import kiln.util.contracts;

namespace kiln::gfx::renderer {

TransferCommandBuffer::TransferCommandBuffer(
    vk::raii::CommandBuffer&& command_buffer
)
    : m_command_buffer{ std::move(command_buffer) }
{
}

auto TransferCommandBuffer::get() const noexcept -> const vk::raii::CommandBuffer&
{
    return m_command_buffer;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto TransferCommandBuffer::begin() -> void
{
    m_command_buffer.begin(
        vk::CommandBufferBeginInfo{
            .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        }
    );
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto TransferCommandBuffer::end() -> void
{
    m_command_buffer.end();
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto TransferCommandBuffer::enqueue_buffer_copy(
    const Buffer& source,
    const Buffer& destination
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

    m_command_buffer.copyBuffer2(info);
}

}   // namespace kiln::gfx::renderer
