module;

#include <optional>

#include <glm/ext/matrix_float4x4.hpp>

#include <fastgltf/core.hpp>

export module examples.frustum_culling.gltf_utils;

import examples.frustum_culling.AABB;

namespace demo {

export [[nodiscard]]
auto apply_vulkan_basis_correction_on_transform(const glm::mat4x4& transform)
    -> glm::mat4x4;


export [[nodiscard]]
auto aabb_from_position_accessor(const fastgltf::Accessor& position_accessor) -> AABB<>;

export [[nodiscard]]
auto aabb_of(const fastgltf::Asset& model, std::size_t scene_index)
    -> std::optional<AABB<>>;

}   // namespace demo
