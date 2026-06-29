module;

#include <concepts>

export module kiln.gfx.vulkan.structure_chain.part_of_comprehensive_feature_struct_c;

import vulkan;

import kiln.gfx.vulkan.structure_chain.comprehensive_feature_struct_c;
import kiln.gfx.vulkan.structure_chain.core_feature_struct_from_vulkan1x_c;
import kiln.gfx.vulkan.structure_chain.individual_feature_struct_c;

namespace kiln::gfx::vulkan {

export template <typename IndividualFeatureStruct_T, typename ComprehensiveFeatureStruct_T>
concept part_of_comprehensive_feature_struct_c
    = individual_feature_struct_c<IndividualFeatureStruct_T>
   && ((std::same_as<ComprehensiveFeatureStruct_T, vk::PhysicalDeviceVulkan11Features>
        && core_feature_struct_from_vulkan11_c<IndividualFeatureStruct_T>)
       || (std::same_as<ComprehensiveFeatureStruct_T, vk::PhysicalDeviceVulkan12Features>
           && core_feature_struct_from_vulkan12_c<IndividualFeatureStruct_T>)
       || (std::same_as<ComprehensiveFeatureStruct_T, vk::PhysicalDeviceVulkan13Features>
           && core_feature_struct_from_vulkan13_c<IndividualFeatureStruct_T>)
       || (std::same_as<ComprehensiveFeatureStruct_T, vk::PhysicalDeviceVulkan14Features>
           && core_feature_struct_from_vulkan14_c<IndividualFeatureStruct_T>));

}   // namespace kiln::gfx::vulkan
