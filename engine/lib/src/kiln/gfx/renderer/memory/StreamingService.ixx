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
import kiln.app.context.ContextBuilderInterface;
import kiln.app.memory.Arena;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.command.TransferCommandPool;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.command.TransferQueueRef;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.util.containers.Indirect;

namespace kiln::gfx::renderer {

export class StreamingService {
public:
    class Builder;

    using allocator_type = std::pmr::polymorphic_allocator<>;


    StreamingService(StreamingService&&, const allocator_type& allocator);

    explicit StreamingService(
        const Device&    device,
        TransferQueueRef host_to_device_transfer_queue
    );
    explicit StreamingService(
        std::allocator_arg_t,
        const allocator_type& allocator,
        const Device&         device,
        TransferQueueRef      host_to_device_transfer_queue
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;


    auto upload(
        std::span<const std::byte> data,
        Buffer&                    destination,
        const Device&              device,
        Allocator&                 allocator
    ) -> void;
    auto flush_uploads(
        TransferQueueRef           transfer_queue,
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
    TransferCommandPool                                    m_staging_command_pool;
    std::pmr::vector<std::pair<TransferCommandBuffer, UploadMetaData>>
        m_standby_staging_command_buffers;
    std::optional<std::pair<TransferCommandBuffer, UploadMetaData>>
        m_recording_staging_command_buffer;
    std::pmr::list<std::pair<TransferCommandBuffer, UploadMetaData>>
                                   m_in_flight_staging_command_buffers;
    std::pmr::forward_list<Buffer> m_staging_buffers;


    [[nodiscard]]
    auto ready_command_buffer_for_staging(const vk::raii::Device& logical_device)
        -> TransferCommandBuffer&;

    auto recycle_executed_staging_command_buffers(
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;
};

class StreamingService::Builder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto
        create(vulkan::InstanceBuilder& instance_builder, DeviceBuilder& device_builder)
            -> Builder;

    [[nodiscard]]
    static auto
        build(app::Arena& arena, const Device& device, QueueProvider& queue_provider)
            -> StreamingService;
};

}   // namespace kiln::gfx::renderer
