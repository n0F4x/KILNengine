module;

#include <cstdint>
#include <functional>

export module kiln.gfx.renderer.command.queue_requests;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.gfx.vulkan.QueueRequestInterface;
import kiln.util.containers.CopyableFunction;
import kiln.wsi.Context;

namespace kiln::gfx::renderer {

export using QueueRequestResultCallback =
    util::CopyableFunction<auto(vulkan::QueueFamilyIndex, uint32_t) const->void>;

export class GraphicsQueueRequest : public vulkan::QueueRequestInterface {
public:
    GraphicsQueueRequest(
        const vk::raii::Instance&    instance,
        const wsi::Context&          wsi_context,
        QueueRequestResultCallback&& result_callback
    );

    [[nodiscard]]
    auto is_suitable(const vk::raii::PhysicalDevice& physical_device) const
        -> bool override;
    auto prepare_queue(
        std::vector<vulkan::QueueFamilyInfo>& current_selection,
        const vk::raii::PhysicalDevice&       physical_device
    ) const -> void override;

private:
    std::reference_wrapper<const vk::raii::Instance> m_instance_ref;
    std::reference_wrapper<const wsi::Context>       m_wsi_context_ref;
    QueueRequestResultCallback                       m_result_callback;


    [[nodiscard]]
    auto is_suitable(
        const vk::raii::PhysicalDevice&   physical_device,
        vulkan::QueueFamilyIndex          family_index,
        const vk::QueueFamilyProperties2& family_properties
    ) const -> bool;
};

}   // namespace kiln::gfx::renderer
