// TODO: remove this module with updated VulkanHPP
export module kiln.gfx.vulkan.format.traits;

import vulkan_hpp;

namespace kiln::gfx::vulkan {

export [[nodiscard]]
constexpr auto has_depth_component(const vk::Format format) -> bool
{
    switch (format)
    {
        using enum vk::Format;
        case eD16Unorm:
        case eX8D24UnormPack32:
        case eD32Sfloat:
        case eD16UnormS8Uint:
        case eD24UnormS8Uint:
        case eD32SfloatS8Uint:  return true;
        default:                return false;
    }
}

}   // namespace kiln::gfx::vulkan
