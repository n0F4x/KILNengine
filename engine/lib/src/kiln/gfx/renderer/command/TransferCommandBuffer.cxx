module;

#include <utility>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.TransferCommandBuffer;

import kiln.util.contracts;

namespace kiln::gfx::renderer {

TransferCommandBuffer::TransferCommandBuffer(
    vk::raii::CommandBuffer&&                     command_buffer,
    const util::EnumMask<CommandBufferUsageFlags> usage_flags
)
    : m_command_buffer{ std::move(command_buffer) },
      m_usage_flags{ usage_flags }
{
}

auto TransferCommandBuffer::get() const noexcept -> const vk::raii::CommandBuffer&
{
    return m_command_buffer;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto TransferCommandBuffer::begin() -> void
{
    vk::CommandBufferUsageFlags flags;
    if (!(m_usage_flags & CommandBufferUsageFlags::eReusable))
    {
        flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    }

    m_command_buffer.begin(
        vk::CommandBufferBeginInfo{
            .flags = flags,
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
