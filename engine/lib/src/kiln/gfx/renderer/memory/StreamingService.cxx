module;

#include <forward_list>
#include <memory_resource>
#include <span>

#include <vk_mem_alloc.h>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.memory.StreamingService;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferUsageFlags;
import kiln.gfx.renderer.command.DependencyInfo;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

StreamingService::StreamingService(
    const Device&          device,
    const TransferQueueRef upload_queue
)
    : StreamingService{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          device,
          upload_queue,
      }
{
}

StreamingService::StreamingService(
    std::allocator_arg_t,
    const allocator_type&  allocator,
    const Device&          device,
    const TransferQueueRef upload_queue
)
    : m_memory_resource{ std::allocator_arg, allocator, allocator.resource() },
      m_upload_queue{ upload_queue },
      m_upload_command_pool{ device, upload_queue.family_index() },
      m_standby_upload_command_buffers{ &*m_memory_resource },
      m_in_flight_upload_command_buffers{ &*m_memory_resource },
      m_staging_buffers{ &*m_memory_resource }
{
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

auto StreamingService::upload(
    const std::span<const std::byte> data,
    Buffer&                          destination,
    const Device&                    device,
    Allocator&                       allocator
) -> void
{
    PRECOND(data.size_bytes() == destination.size());

    if (destination.allocation().memory_properties()
        & vk::MemoryPropertyFlagBits::eHostVisible)
    {
        allocator.host_copy(data, destination);
        return;
    }

    Buffer staging_buffer{ create_staging_buffer(allocator, data.size_bytes()) };
    allocator.host_copy(data, staging_buffer);

    TransferCommandBuffer& staging_command_buffer{
        ready_command_buffer_for_staging(device.logical_device())
    };

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
    staging_command_buffer.barrier(staging_dependencies);
    staging_command_buffer.enqueue_buffer_copy(staging_buffer, destination);
}

[[nodiscard]]
auto create_timeline_semaphore(const vk::raii::Device& logical_device)
    -> vk::raii::Semaphore
{
    constexpr static vk::SemaphoreTypeCreateInfo semaphore_type{
        .semaphoreType = vk::SemaphoreType::eTimeline,
    };
    constexpr static vk::SemaphoreCreateInfo create_info{
        .pNext = &semaphore_type,
    };
    return vulkan::check_result(logical_device.createSemaphore(create_info));
}

auto StreamingService::flush(
    const SubmitInfo&          submit_info,
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    if (m_recording_upload_command_buffer.has_value())
    {
        m_recording_upload_command_buffer->first.end();

        std::pmr::vector<vk::SemaphoreSubmitInfo> signal_semaphores{
            &transient_memory_resource
        };
        signal_semaphores.reserve(submit_info.signal_semaphores.size() + 1);
        signal_semaphores.append_range(submit_info.signal_semaphores);
        signal_semaphores.push_back(
            vk::SemaphoreSubmitInfo{
                .semaphore =
                    m_recording_upload_command_buffer->second.binary_timeline_semaphore,
                .value     = m_recording_upload_command_buffer->second.waited_on_value,
                .stageMask = vk::PipelineStageFlagBits2::eCopy,
            }
        );

        const SubmitInfo submit_info_with_our_semaphore{
            .wait_semaphores   = submit_info.wait_semaphores,
            .signal_semaphores = signal_semaphores,
            .fence             = submit_info.fence,
        };
        m_upload_queue.submit(
            m_recording_upload_command_buffer->first, submit_info_with_our_semaphore
        );

        recycle_executed_upload_command_buffers(transient_memory_resource);

        m_in_flight_upload_command_buffers.push_back(
            std::move(*m_recording_upload_command_buffer)
        );
        m_recording_upload_command_buffer.reset();
    }
}

auto StreamingService::ready_command_buffer_for_staging(
    const vk::raii::Device& logical_device
) -> TransferCommandBuffer&
{
    if (!m_recording_upload_command_buffer.has_value())
    {
        if (m_standby_upload_command_buffers.empty())
        {
            m_recording_upload_command_buffer = std::pair{
                m_upload_command_pool.allocate_primary(CommandBufferUsageFlags::eReusable),
                UploadMetaData{ create_timeline_semaphore(logical_device) },
            };
        }
        else
        {
            m_recording_upload_command_buffer =
                std::move(m_standby_upload_command_buffers.back());
            m_standby_upload_command_buffers.pop_back();
        }

        m_recording_upload_command_buffer->first.begin();
    }

    return m_recording_upload_command_buffer->first;
}

auto StreamingService::recycle_executed_upload_command_buffers(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    std::pmr::forward_list<decltype(m_in_flight_upload_command_buffers)::iterator>
        finished_queue{ &transient_memory_resource };

    for (auto iter{ m_in_flight_upload_command_buffers.begin() };
         iter != m_in_flight_upload_command_buffers.end();
         ++iter)
    {
        if (iter->second.binary_timeline_semaphore.getCounterValue()
            >= iter->second.waited_on_value)
        {
            ++iter->second.waited_on_value;
            finished_queue.push_front(iter);
        }
    }

    for (const auto& iter : finished_queue)
    {
        m_standby_upload_command_buffers.push_back(std::move(*iter));
        m_in_flight_upload_command_buffers.erase(iter);
    }
}

auto StreamingService::Builder::create(
    DeviceBuilder&        device_builder,
    QueueProviderBuilder& queue_provider_builder
) -> Builder
{
    device_builder.enable_features(
        vk::PhysicalDeviceTimelineSemaphoreFeatures{
            .timelineSemaphore = vk::True,
        }
    );

    queue_provider_builder.require_host_to_device_transfer_queue();

    return Builder{};
}

}   // namespace kiln::gfx::renderer
