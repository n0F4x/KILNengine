module;

#include <format>
#include <span>
#include <utility>

#include <vk_mem_alloc.h>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.stream.StagingStream;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferUsageFlags;
import kiln.gfx.renderer.command.DependencyInfo;
import kiln.gfx.renderer.command.TransferCommandPool;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;
import kiln.util.reflection;

namespace kiln::gfx::renderer {

StagingStream::StagingStream(StagingStream&& other, const allocator_type& allocator)
    : m_queue_ref{ other.m_queue_ref },
      m_command_pool{ std::move(other.m_command_pool) },
      m_command_buffer{ std::move(other.m_command_buffer) },
      m_memory_pool{ std::move(other.m_memory_pool), allocator },
      m_staging_regions{ std::move(other.m_staging_regions), &*m_memory_pool },
      m_number_of_staging_regions{ other.m_number_of_staging_regions },
      m_in_flight_staging_buffers{
          std::move(other.m_in_flight_staging_buffers),
          &*m_memory_pool,
      },
      m_staging_finished_fence{ std::move(other.m_staging_finished_fence) }
{
}

StagingStream::StagingStream(const Device& device, TransferQueue& queue)
    : StagingStream{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          device,
          queue,
      }
{
}

[[nodiscard]]
auto create_fence(const vk::raii::Device& device) -> vk::raii::Fence
{
    constexpr static vk::FenceCreateInfo create_info{};
    return vulkan::check_result(device.createFence(create_info));
}

StagingStream::StagingStream(
    std::allocator_arg_t,
    const allocator_type& allocator,
    const Device&         device,
    TransferQueue&        queue
)
    : m_queue_ref{ queue },
      m_command_pool{ device, queue.family_index() },
      m_command_buffer{
          m_command_pool.allocate_primary(CommandBufferUsageFlags::eReusable)
      },
      m_memory_pool{ std::allocator_arg, allocator, allocator.resource() },
      m_staging_finished_fence{ create_fence(device.logical_device()) }
{
}

auto StagingStream::get_allocator() const noexcept -> allocator_type
{
    return m_memory_pool.get_allocator();
}

auto StagingStream::empty() const noexcept -> bool
{
    return m_number_of_staging_regions == 0;
}

auto StagingStream::record(const StagingRegion& staging_regions) -> void
{
    PRECOND(
        m_in_flight_staging_buffers.empty(),
        std::format(
            "`{}` must be reset before it can begin recording",
            util::name_of<StagingStream>()
        )
    );

    m_staging_regions.push_front(staging_regions);
    ++m_number_of_staging_regions;
}

auto StagingStream::reset(const Device& device) -> void
{
    vulkan::check_result(device.logical_device().waitForFences(
        *m_staging_finished_fence, vk::True, std::numeric_limits<uint64_t>::max()
    ));
    device.logical_device().resetFences(*m_staging_finished_fence);

    m_command_pool.reset();
    m_staging_regions.clear();
    m_number_of_staging_regions = 0;
    m_in_flight_staging_buffers.clear();
}

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

auto stage(
    Buffer&                staging_buffer,
    TransferCommandBuffer& command_buffer,
    const StagingRegion&   staging_region,
    Allocator&             allocator
) -> void
{
    allocator.host_copy(staging_region.data(), staging_buffer);

    const vk::BufferMemoryBarrier2 staging_buffer_barrier{
        .srcStageMask  = vk::PipelineStageFlagBits2::eHost,
        .srcAccessMask = vk::AccessFlagBits2::eHostWrite,
        .dstStageMask  = vk::PipelineStageFlagBits2::eTransfer,
        .dstAccessMask = vk::AccessFlagBits2::eTransferRead,
        .buffer        = staging_region.destination().buffer(),
        .offset        = staging_region.destination().offset(),
        .size          = staging_region.destination().size(),
    };
    const DependencyInfo staging_dependencies{
        .buffer_memory_barriers = std::span{ &staging_buffer_barrier, 1 },
    };
    command_buffer.record_barrier(staging_dependencies);

    command_buffer.record_buffer_copy(staging_buffer, staging_region.destination());
}

auto StagingStream::flush(Allocator& allocator, SubmitInfo&& submit_info) -> void
{
    PRECOND(!empty());
    PRECOND(
        !submit_info.fence().has_value(),
        std::format(
            "Use `{}::reset()` instead of another fence", util::name_of<StagingStream>()
        )
    );

    m_command_buffer.begin_recording();
    for (const StagingRegion& staging_region : m_staging_regions)
    {
        // TODO: optimization: merge staging buffers
        m_in_flight_staging_buffers.push_front(
            create_staging_buffer(allocator, staging_region.size())
        );

        stage(
            m_in_flight_staging_buffers.front(), m_command_buffer, staging_region, allocator
        );
    }
    m_command_buffer.end_recording();

    submit_info.fence() = *m_staging_finished_fence;

    m_queue_ref.get().submit(m_command_buffer, submit_info);
}

}   // namespace kiln::gfx::renderer
