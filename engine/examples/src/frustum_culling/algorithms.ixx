module;

#include <span>

#include <glm/ext/vector_float3.hpp>

export module examples.frustum_culling.algorithms;

import examples.frustum_culling.shaders;

namespace demo {

export [[nodiscard]]
auto welzl(std::span<const glm::vec3> points) -> shaders::SBV;

}   // namespace demo
