module;

#include <concepts>

export module kiln.gfx.vulkan.structure_chain.vulkan1x_feature_struct_c;

import vulkan;

namespace kiln::gfx::vulkan {

export template <typename T>
concept vulkan1x_feature_struct_c = std::same_as<T, vk::PhysicalDeviceVulkan11Features>
                                 || std::same_as<T, vk::PhysicalDeviceVulkan12Features>
                                 || std::same_as<T, vk::PhysicalDeviceVulkan13Features>
                                 || std::same_as<T, vk::PhysicalDeviceVulkan14Features>;

}   // namespace kiln::gfx::vulkan
