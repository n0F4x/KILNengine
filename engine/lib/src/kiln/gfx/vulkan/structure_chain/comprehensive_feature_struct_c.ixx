module;

#include <concepts>

export module kiln.gfx.vulkan.structure_chain.comprehensive_feature_struct_c;

import vulkan_hpp;

import kiln.gfx.vulkan.structure_chain.vulkan1x_feature_struct_c;

namespace kiln::gfx::vulkan {

export template <typename T>
concept comprehensive_feature_struct_c
    = std::same_as<T, vk::PhysicalDeviceFeatures> || vulkan1x_feature_struct_c<T>;

}   // namespace kiln::gfx::vulkan
