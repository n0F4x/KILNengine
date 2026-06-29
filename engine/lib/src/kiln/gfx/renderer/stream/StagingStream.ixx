module;

#include <cstddef>
#include <forward_list>
#include <memory_resource>

export module kiln.gfx.renderer.stream.StagingStream;

import vulkan;

import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.command.TransferCommandPool;
import kiln.gfx.renderer.command.TransferQueueRef;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.stream.StagingRequest;
import kiln.util.containers.Indirect;

namespace kiln::gfx::renderer {

export class StagingStream {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    StagingStream(const StagingStream&) = delete;
    StagingStream(StagingStream&&)      = default;
    StagingStream(StagingStream&&, const allocator_type&);

    explicit StagingStream(const Device& device, TransferQueueRef queue);
    explicit StagingStream(
        std::allocator_arg_t,
        const allocator_type& allocator,
        const Device&         device,
        TransferQueueRef      queue
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto empty() const noexcept -> bool;

    auto record(StagingRequest&& staging_request) -> void;

    /*
     * Waits for flush
     */
    auto reset(const Device& device) -> void;
    auto flush(Allocator& allocator, SubmitInfo&& submit_info = SubmitInfo{}) -> void;

private:
    TransferQueueRef                                       m_queue;
    TransferCommandPool                                    m_command_pool;
    TransferCommandBuffer                                  m_command_buffer;
    util::Indirect<std::pmr::unsynchronized_pool_resource> m_memory_pool;
    std::pmr::forward_list<StagingRequest> m_staging_requests{ &*m_memory_pool };
    std::size_t                            m_number_of_staging_requests{};
    std::pmr::forward_list<Buffer>         m_in_flight_staging_buffers{ &*m_memory_pool };
    vk::raii::Fence                        m_staging_finished_fence;
};

}   // namespace kiln::gfx::renderer
