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
import kiln.gfx.renderer.command.GraphicsQueueRef;
import kiln.gfx.renderer.command.TransferQueueRef;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class QueueProvider {
public:
    class Builder;

    struct QueuePack {
        QueueType                type;
        vulkan::QueueFamilyIndex family_index;
        vk::QueueFlags           flags;
        uint32_t                 index;
        vk::raii::Queue          queue;
    };

    struct Queues {
        std::optional<QueuePack> graphics_queue_pack;
        std::optional<QueuePack> host_to_device_transfer_queue_pack;
    };

    explicit QueueProvider(Queues&& queues);


    [[nodiscard]]
    auto graphics_queue() [[kiln_lifetimebound]] -> std::optional<GraphicsQueueRef>;
    [[nodiscard]]
    auto host_to_device_transfer_queue() [[kiln_lifetimebound]]
    -> std::optional<TransferQueueRef>;

    template <typename RankFunc_T>
        requires std::
            same_as<std::invoke_result_t<RankFunc_T, QueueType>, std::optional<uint32_t>>
        [[nodiscard]]
        auto select_transfer_queue(RankFunc_T&& rank_func)
            -> std::optional<TransferQueueRef>;

private:
    Queues m_queues;


    [[nodiscard]]
    auto available_queues() -> std::vector<std::reference_wrapper<QueuePack>>;
};

class QueueProvider::Builder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto build(const Device& device) -> QueueProvider;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

template <typename RankFunc_T>
    requires std::
        same_as<std::invoke_result_t<RankFunc_T, QueueType>, std::optional<unsigned>>
    auto QueueProvider::select_transfer_queue(RankFunc_T&& rank_func)
        -> std::optional<TransferQueueRef>
{
    std::optional<TransferQueueRef> result;
    std::optional<uint32_t>         highest_rank;
    for (QueuePack& queue_pack : available_queues())
    {
        if (queue_pack.flags & vk::QueueFlagBits::eTransfer)
        {
            if (const std::optional<uint32_t> rank{
                    std::invoke(rank_func, queue_pack.type) };
                rank.has_value() && (!highest_rank.has_value() || *rank > *highest_rank))
            {
                result = TransferQueueRef{
                    queue_pack.family_index,
                    queue_pack.queue,
                };
                highest_rank = rank;
            }
        }
    }

    return result;
}

}   // namespace kiln::gfx::renderer
