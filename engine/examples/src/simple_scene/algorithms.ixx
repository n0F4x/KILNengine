module;

#include <span>

#include <glm/ext/vector_float3.hpp>

export module examples.simple_scene.algorithms;

import examples.simple_scene.shaders;

namespace demo {

export [[nodiscard]]
auto welzl(std::span<const glm::vec3> points) -> shaders::SBV;

}   // namespace demo
