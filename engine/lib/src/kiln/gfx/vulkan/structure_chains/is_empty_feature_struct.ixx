module;

#include <utility>

export module kiln.gfx.vulkan.structure_chains.is_empty_feature_struct;

import vulkan_hpp;

import kiln.gfx.vulkan.structure_chains.feature_struct_c;

namespace kiln::gfx::vulkan {

export template <feature_struct_c FeatureStruct_T>
constexpr auto is_empty_feature_struct(const FeatureStruct_T& feature_struct) -> bool;

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

constexpr auto is_turned_on_flag(const vk::StructureType) -> bool
{
    return false;
}

constexpr auto is_turned_on_flag(void* const) -> bool
{
    return false;
}

constexpr auto is_turned_on_flag(const vk::Bool32& flag) -> bool
{
    return flag;
}

template <feature_struct_c FeatureStruct_T>
constexpr auto is_empty_feature_struct(const FeatureStruct_T& feature_struct) -> bool
{
    const auto& [... members]{ feature_struct };
    return !(is_turned_on_flag(members) || ...);
}

}   // namespace kiln::gfx::vulkan
