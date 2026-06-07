module;

#include <span>

#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

module examples.frustum_culling.algorithms;

import examples.frustum_culling.shaders;

namespace demo {

[[nodiscard]]
auto boundary_sphere_from(const std::span<const glm::vec3> boundary) -> shaders::SBV
{
    if (boundary.empty())
    {
        return shaders::SBV{
            .center = glm::vec3{ 0, 0, 0 },
            .radius = 0
        };
    }
    if (boundary.size() == 1)
    {
        return shaders::SBV{ .center = boundary.front(), .radius = 0 };
    }
    if (boundary.size() == 2)
    {
        return shaders::SBV{
            .center = (boundary[0] + boundary[1]) * 0.5f,
            .radius = glm::distance(boundary[0], boundary[1]) * 0.5f,
        };
    }
    if (boundary.size() == 3)
    {
        const glm::vec3& a{ boundary[0] };
        const glm::vec3& b{ boundary[1] };
        const glm::vec3& c{ boundary[2] };

        const glm::vec3 ac{ c - a };
        const glm::vec3 ab{ b - a };

        const glm::vec3 ab_x_ac{ glm::cross(ab, ac) };

        const glm::vec3 to_center{
            (glm::cross(ab_x_ac, ab) * glm::dot(ac, ac)
             + glm::cross(ac, ab_x_ac) * glm::dot(ab, ab))
                / (2.0f * glm::dot(ab_x_ac, ab_x_ac)),
        };

        return shaders::SBV{ .center = a + to_center, .radius = glm::length(to_center) };
    }

    const glm::vec3& a{ boundary[0] };
    const glm::vec3& b{ boundary[1] };
    const glm::vec3& c{ boundary[2] };
    const glm::vec3& d{ boundary[3] };

    const glm::mat3 A{ b - a, c - a, d - a };
    const glm::vec3 b_vec{
        0.5f
            * glm::vec3(
                glm::dot(b, b) - glm::dot(a, a),
                glm::dot(c, c) - glm::dot(a, a),
                glm::dot(d, d) - glm::dot(a, a)
            ),
    };

    if (std::abs(glm::determinant(A)) < 1e-6f)
    {
        return shaders::SBV{ .center = a, .radius = 0 };
    }

    const glm::vec3 center{ glm::inverse(glm::transpose(A)) * b_vec };

    return shaders::SBV{ .center = center, .radius = glm::distance(center, a) };
}

[[nodiscard]]
auto is_inside(const shaders::SBV& sphere, const glm::vec3& point) -> bool
{
    return glm::distance2(sphere.center, point) <= sphere.radius * sphere.radius + 1e-6f;
}

// TODO: use inplace_vector
[[nodiscard]]
auto welzl(
    const std::span<const glm::vec3> points,
    std::array<glm::vec3, 4>&        boundary,
    uint32_t                         boundary_size
) -> shaders::SBV
{
    if (points.empty() || boundary_size == 4)
    {
        return boundary_sphere_from(std::span{ boundary }.subspan(0, boundary_size));
    }

    const glm::vec3 current_point{ points.front() };
    const std::span remaining{ points.subspan(1) };

    if (const shaders::SBV sphere_candidate{ welzl(remaining, boundary, boundary_size) };
        is_inside(sphere_candidate, current_point))
    {
        return sphere_candidate;
    }

    boundary[boundary_size] = current_point;
    ++boundary_size;

    const shaders::SBV result{ welzl(remaining, boundary, boundary_size) };

    --boundary_size;

    return result;
}

[[nodiscard]]
auto welzl(const std::span<const glm::vec3> points) -> shaders::SBV
{
    std::array<glm::vec3, 4> boundary{};
    return welzl(points, boundary, 0);
}

}   // namespace demo
