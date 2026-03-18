module;

#include <cstdint>
#include <optional>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.command.QueueProvider;

import vulkan_hpp;

import kiln.gfx.renderer.command.GraphicsQueueRef;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class QueueProvider {
public:
    struct QueuePack {
        vulkan::QueueFamilyIndex family_index;
        uint32_t                 index;
        vk::raii::Queue          queue;
    };

    struct Queues {
        std::optional<QueuePack> graphics_queue_pack;
    };

    explicit QueueProvider(const Queues& queues);

    [[nodiscard]]
    auto graphics_queue() const [[kiln_lifetimebound]] -> std::optional<GraphicsQueueRef>;

private:
    Queues m_queues;
};

}   // namespace kiln::gfx::renderer
