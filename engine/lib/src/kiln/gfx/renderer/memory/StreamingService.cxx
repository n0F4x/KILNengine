module;

#include <cstddef>
#include <span>

#include <vk_mem_alloc.h>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.memory.StreamingService;

import vulkan_hpp;

import kiln.gfx.renderer.command.DependencyInfo;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

[[nodiscard]]
auto create_staging_buffer(Allocator& allocator, const vk::DeviceSize buffer_size)
    -> Buffer
{
    constexpr static vk::BufferUsageFlags2CreateInfo buffer_usage_flags{
        .usage = vk::BufferUsageFlagBits2::eTransferSrc,
    };
    const vk::BufferCreateInfo buffer_create_info{
        .pNext = &buffer_usage_flags,
        .size  = buffer_size,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    return allocator.create_buffer(buffer_create_info, allocation_create_info);
}

auto StreamingService::stage(
    Allocator&                       allocator,
    const std::span<const std::byte> data,
    Buffer&                          destination,
    TransferCommandBuffer&           command_buffer
) -> void
{
    PRECOND(data.size_bytes() == destination.size());
    PRECOND(
        !(destination.allocation().memory_properties()
          & vk::MemoryPropertyFlagBits::eHostVisible),
        "Use host copy instead of staging!"
    );

    Buffer staging_buffer{ create_staging_buffer(allocator, data.size_bytes()) };
    allocator.host_copy(data, staging_buffer);

    const vk::BufferMemoryBarrier2 staging_buffer_barrier{
        .srcStageMask  = vk::PipelineStageFlagBits2::eHost,
        .srcAccessMask = vk::AccessFlagBits2::eHostWrite,
        .dstStageMask  = vk::PipelineStageFlagBits2::eTransfer,
        .dstAccessMask = vk::AccessFlagBits2::eTransferRead,
        .buffer        = staging_buffer.get(),
        .offset        = 0,
        .size          = staging_buffer.size(),
    };
    const DependencyInfo staging_dependencies{
        .buffer_memory_barriers = std::span{ &staging_buffer_barrier, 1 },
    };
    command_buffer.record_barrier(staging_dependencies);
    command_buffer.record_buffer_copy(staging_buffer, destination);
}

}   // namespace kiln::gfx::renderer
