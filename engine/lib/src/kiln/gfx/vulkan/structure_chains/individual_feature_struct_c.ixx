export module kiln.gfx.vulkan.structure_chains.individual_feature_struct_c;

import vulkan_hpp;

import kiln.gfx.vulkan.structure_chains.extends_struct_c;
import kiln.gfx.vulkan.structure_chains.comprehensive_feature_struct_c;

namespace kiln::gfx::vulkan {

export template <typename T>
concept individual_feature_struct_c = extends_struct_c<T, vk::PhysicalDeviceFeatures2>
                                   || !comprehensive_feature_struct_c<T>;

}   // namespace kiln::gfx::vulkan
