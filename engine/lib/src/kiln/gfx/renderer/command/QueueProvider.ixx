module;

#include <concepts>
#include <cstdint>
#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.command.QueueProvider;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.command.ComputeQueue;
import kiln.gfx.renderer.command.GraphicsQueue;
import kiln.gfx.renderer.command.TransferQueue;
import kiln.gfx.renderer.command.QueueBase;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

export class QueueProvider {
public:
    class Builder;

    struct Queues {
        std::optional<GraphicsQueue> graphics_queue_pack;
        std::optional<ComputeQueue>  compute_queue_pack;
        std::optional<TransferQueue> host_to_device_transfer_queue_pack;
    };

    explicit QueueProvider(Queues&& queues);


    [[nodiscard]]
    auto graphics_queue() [[kiln_lifetimebound]] -> util::OptionalRef<GraphicsQueue>;
    [[nodiscard]]
    auto compute_queue() [[kiln_lifetimebound]] -> util::OptionalRef<ComputeQueue>;
    [[nodiscard]]
    auto host_to_device_transfer_queue() [[kiln_lifetimebound]]
    -> util::OptionalRef<TransferQueue>;

    template <typename RankFunc_T>
        requires std::
            same_as<std::invoke_result_t<RankFunc_T, QueueBase>, std::optional<uint32_t>>
        [[nodiscard]]
        auto select_queue(RankFunc_T&& rank_func) -> util::OptionalRef<QueueBase>;

private:
    Queues m_queues;


    [[nodiscard]]
    auto available_queues() -> std::vector<std::reference_wrapper<QueueBase>>;
};

class QueueProvider::Builder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto create(
        vulkan::InstanceBuilder& instance_builder,
        DeviceBuilder&           device_builder
    ) -> Builder;

    [[nodiscard]]
    static auto build(const Device& device) -> QueueProvider;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

template <typename RankFunc_T>
    requires std::
        same_as<std::invoke_result_t<RankFunc_T, QueueBase>, std::optional<uint32_t>>
    auto QueueProvider::select_queue(RankFunc_T&& rank_func)
        -> util::OptionalRef<QueueBase>
{
    util::OptionalRef<QueueBase> result;
    std::optional<uint32_t>      highest_rank;
    for (QueueBase& queue : available_queues())
    {
        if (const std::optional<uint32_t> rank{ std::invoke(rank_func, queue) };
            rank.has_value() && (!highest_rank.has_value() || *rank > *highest_rank))
        {
            result       = queue;
            highest_rank = rank;
        }
    }

    return result;
}

}   // namespace kiln::gfx::renderer
