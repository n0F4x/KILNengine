module;

#include <forward_list>
#include <list>
#include <memory_resource>
#include <optional>
#include <span>
#include <vector>

export module kiln.gfx.renderer.memory.StreamingService;

import vulkan_hpp;

import kiln.app.Builder;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.command.QueueProviderBuilder;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.command.TransferCommandPool;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.command.TransferQueueRef;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;
import kiln.util.containers.Indirect;

namespace kiln::gfx::renderer {

export class StreamingService {
public:
    class Builder;

    using allocator_type = std::pmr::polymorphic_allocator<>;


    explicit StreamingService(const Device& device, TransferQueueRef upload_queue);
    explicit StreamingService(
        std::allocator_arg_t,
        const allocator_type& allocator,
        const Device&         device,
        TransferQueueRef      upload_queue
    );


    auto upload(
        std::span<const std::byte> data,
        Buffer&                    destination,
        const Device&              device,
        Allocator&                 allocator
    ) -> void;
    auto flush(
        const SubmitInfo&          submit_info,
        std::pmr::memory_resource& transient_memory_resource =
            *std::pmr::get_default_resource()
    ) -> void;

private:
    struct UploadMetaData {
        vk::raii::Semaphore binary_timeline_semaphore;
        uint64_t            waited_on_value{ 1 };
    };

    util::Indirect<std::pmr::unsynchronized_pool_resource> m_memory_resource;
    TransferQueueRef                                       m_upload_queue;
    TransferCommandPool                                    m_upload_command_pool;
    std::pmr::vector<std::pair<TransferCommandBuffer, UploadMetaData>>
        m_standby_upload_command_buffers;
    std::optional<std::pair<TransferCommandBuffer, UploadMetaData>>
        m_recording_upload_command_buffer;
    std::pmr::list<std::pair<TransferCommandBuffer, UploadMetaData>>
                                   m_in_flight_upload_command_buffers;
    std::pmr::forward_list<Buffer> m_staging_buffers;


    [[nodiscard]]
    auto ready_command_buffer_for_staging(const vk::raii::Device& logical_device)
        -> TransferCommandBuffer&;

    auto recycle_executed_upload_command_buffers(
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;
};

class StreamingService::Builder {
public:
    [[nodiscard]]
    static auto create(
        DeviceBuilder&        device_builder,
        QueueProviderBuilder& queue_provider_builder
    ) -> Builder;
};

}   // namespace kiln::gfx::renderer
