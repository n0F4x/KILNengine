module;

#include <concepts>

export module kiln.gfx.vulkan.structure_chains.feature_struct_c;

import vulkan_hpp;

import kiln.gfx.vulkan.structure_chains.extends_struct_c;

namespace kiln::gfx::vulkan {

export template <typename T>
concept feature_struct_c = std::same_as<T, vk::PhysicalDeviceFeatures>
                        || extends_struct_c<T, vk::PhysicalDeviceFeatures2>;

}   // namespace kiln::gfx::vulkan
