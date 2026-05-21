module;

#include <optional>

#include <fastgltf/core.hpp>

export module examples.simple_scene.gltf_utils;

import examples.simple_scene.AABB;

namespace demo {

export [[nodiscard]]
auto bounding_box_of(const fastgltf::Asset& model, std::size_t scene_index)
    -> std::optional<AABB<>>;

}   // namespace demo
