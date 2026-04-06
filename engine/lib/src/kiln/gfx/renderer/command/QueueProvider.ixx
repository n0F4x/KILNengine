module;

#include <cstdint>
#include <functional>
#include <optional>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.command.QueueProvider;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.renderer.command.GraphicsQueueRef;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.renderer.presentation.PresentationContextBuilder;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

namespace internal {

export class QueueProviderBuilder;

}   // namespace internal

export class QueueProvider {
public:
    using Builder = internal::QueueProviderBuilder;

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

namespace internal {

struct QueueInfo {
    bool                                                              requested{};
    std::optional<std::pair<vulkan::QueueFamilyIndex, std::uint32_t>> callback_result;
};

export class QueueProviderBuilder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto create(
        const PresentationContextBuilder& presentation_context_builder,
        DeviceBuilder&                    device_builder
    ) -> QueueProviderBuilder;

    explicit QueueProviderBuilder(DeviceBuilder& device_builder);

    auto require_graphics_queue() -> void;

    [[nodiscard]]
    auto build(const Device& device) const -> QueueProvider;

private:
    std::reference_wrapper<DeviceBuilder> m_device_builder_ref;

    QueueInfo m_graphics_queue_info;
};

}   // namespace internal

}   // namespace kiln::gfx::renderer
