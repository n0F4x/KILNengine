module;

#include <array>
#include <cstddef>
#include <span>
#include <utility>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <fastgltf/core.hpp>

#include "kiln/util/contract_macros.hpp"

module examples.simple_scene.workflow.Asset;

import kiln.util.contracts;

namespace demo {

Asset::Asset(fastgltf::Asset&& asset) : m_asset{ std::move(asset) } {}

using Index = uint32_t;
constexpr static std::array indices{
    Index{ 0 },
    Index{ 1 },
    Index{ 2 },
};

using Position = glm::vec3;
constexpr static std::array positions{
    Position{   0.f, -0.5f, 0.f },
    Position{  0.5f,  0.5f, 0.f },
    Position{ -0.5f,  0.5f, 0.f },
};

struct RestOfVertex {
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 uv_1;
    glm::vec2 uv_2;
    glm::vec3 color;
};

constexpr static std::array rest_of_vertices{
    RestOfVertex{},
    RestOfVertex{},
    RestOfVertex{},
};

auto Asset::indices_size_bytes() const noexcept -> uint32_t
{
    return indices.size() * sizeof(Index);
}

auto Asset::positions_size_bytes() const noexcept -> uint32_t
{
    return positions.size() * sizeof(Position);
}

auto Asset::rest_of_vertices_size_bytes() const noexcept -> uint32_t
{
    return rest_of_vertices.size() * sizeof(RestOfVertex);
}

auto Asset::lazy_indices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            PRECOND(out.size() == indices_size_bytes());
            std::memcpy(out.data(), indices.data(), out.size());
        }   //
    };
}

auto Asset::lazy_positions_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            PRECOND(out.size() == positions_size_bytes());
            std::memcpy(out.data(), positions.data(), out.size());
        }   //
    };
}

auto Asset::lazy_rest_of_vertices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            PRECOND(out.size() == rest_of_vertices_size_bytes());
            std::memcpy(out.data(), rest_of_vertices.data(), out.size());
        }   //
    };
}

}   // namespace demo
