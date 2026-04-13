module;

#include <cstddef>
#include <forward_list>
#include <functional>
#include <memory_resource>
#include <span>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.stream.StagingStream;

import vulkan_hpp;

import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.command.TransferCommandPool;
import kiln.gfx.renderer.command.TransferQueue;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.stream.StagingRegion;
import kiln.util.containers.Indirect;

namespace kiln::gfx::renderer {

export class StagingStream {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    StagingStream(const StagingStream&) = delete;
    StagingStream(StagingStream&&)      = default;
    StagingStream(StagingStream&&, const allocator_type&);

    explicit StagingStream(
        const Device&                         device,
        [[kiln_lifetimebound]] TransferQueue& queue
    );
    explicit StagingStream(
        std::allocator_arg_t,
        const allocator_type&                 allocator,
        const Device&                         device,
        [[kiln_lifetimebound]] TransferQueue& queue
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto empty() const noexcept -> bool;

    auto record(const StagingRegion& staging_regions) -> void;

    /*
     * Waits for flush
     */
    auto reset(const Device& device) -> void;
    auto flush(Allocator& allocator, SubmitInfo&& submit_info = SubmitInfo{}) -> void;

private:
    std::reference_wrapper<TransferQueue>                  m_queue_ref;
    TransferCommandPool                                    m_command_pool;
    TransferCommandBuffer                                  m_command_buffer;
    util::Indirect<std::pmr::unsynchronized_pool_resource> m_memory_pool;
    std::pmr::forward_list<StagingRegion> m_staging_regions{ &*m_memory_pool };
    std::size_t                           m_number_of_staging_regions{};
    std::pmr::forward_list<Buffer>        m_in_flight_staging_buffers{ &*m_memory_pool };
    vk::raii::Fence                       m_staging_finished_fence;
};

}   // namespace kiln::gfx::renderer
