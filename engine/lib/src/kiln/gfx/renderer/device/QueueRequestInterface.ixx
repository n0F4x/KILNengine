module;

#include <vector>

export module kiln.gfx.renderer.device.QueueRequestInterface;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyInfo;

namespace kiln::gfx::renderer {

export class QueueRequestInterface {
public:
    virtual ~QueueRequestInterface() = default;

    [[nodiscard]]
    virtual auto is_suitable(const vk::raii::PhysicalDevice& physical_device) const
        -> bool = 0;

    virtual auto prepare_queue(
        std::vector<vulkan::QueueFamilyInfo>& current_selection,
        const vk::raii::PhysicalDevice&       physical_device
    ) const -> void = 0;
};

}   // namespace kiln::gfx::renderer
