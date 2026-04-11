module;

#include <cstdint>
#include <functional>
#include <optional>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.command.QueueProvider;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.command.GraphicsQueueRef;
import kiln.gfx.renderer.command.TransferQueueRef;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class QueueProvider {
public:
    class Builder;

    struct QueuePack {
        vulkan::QueueFamilyIndex family_index;
        uint32_t                 index;
        vk::raii::Queue          queue;
    };

    struct Queues {
        std::optional<QueuePack> graphics_queue_pack;
        std::optional<QueuePack> host_to_device_transfer_queue_pack;
    };

    explicit QueueProvider(Queues&& queues);


    [[nodiscard]]
    auto graphics_queue() const [[kiln_lifetimebound]] -> std::optional<GraphicsQueueRef>;
    [[nodiscard]]
    auto host_to_device_transfer_queue() const [[kiln_lifetimebound]]
    -> std::optional<TransferQueueRef>;

private:
    Queues m_queues;
};

class QueueProvider::Builder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto build(const Device& device) -> QueueProvider;
};

}   // namespace kiln::gfx::renderer
