module;

#include <array>
#include <cstddef>
#include <limits>
#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <fastgltf/core.hpp>

#include "kiln/util/contract_macros.hpp"

module examples.simple_scene.workflow.Asset;

import kiln.util.contracts;

import examples.simple_scene.shaders;

namespace demo {

AssetLoader::AssetLoader(const fastgltf::Asset& asset) : m_asset{ asset } {}

using Index = uint32_t;
constexpr static std::array indices{
    Index{ 0 },
    Index{ 1 },
    Index{ 2 },
};

constexpr static std::array positions{
    glm::vec3{   0.f, -0.5f, 0.f },
    glm::vec3{  0.5f,  0.5f, 0.f },
    glm::vec3{ -0.5f,  0.5f, 0.f },
};

struct ShaderVertex {
    glm::vec4 color;
};

constexpr static std::array vertices{
    ShaderVertex{ .color{ 1, 0, 0, 1 } },
    ShaderVertex{ .color{ 0, 1, 0, 1 } },
    ShaderVertex{ .color{ 0, 0, 1, 1 } },
};

auto AssetLoader::indices_size_bytes() const noexcept -> uint32_t
{
    return indices.size() * sizeof(Index);
}

auto AssetLoader::positions_size_bytes() const noexcept -> uint32_t
{
    return positions.size() * sizeof(glm::vec3);
}

auto AssetLoader::vertices_size_bytes() const noexcept -> uint32_t
{
    return vertices.size() * sizeof(ShaderVertex);
}

auto AssetLoader::materials_size_bytes() const noexcept -> uint32_t
{
    return 0;
}

auto AssetLoader::draw_command_size_bytes() const noexcept -> uint32_t
{
    return sizeof(shaders::DrawCommand);
}

auto AssetLoader::index_alignment() const noexcept -> uint32_t
{
    return alignof(Index);
}

auto AssetLoader::position_alignment() const noexcept -> uint32_t
{
    return alignof(glm::vec3);
}

auto AssetLoader::vertex_alignment() const noexcept -> uint32_t
{
    return alignof(ShaderVertex);
}

auto AssetLoader::material_alignment() const noexcept -> uint32_t
{
    return alignof(shaders::Material);
}

auto AssetLoader::draw_command_alignment() const noexcept -> uint32_t
{
    return alignof(shaders::DrawCommand);
}

auto AssetLoader::set_index_offset(const uint32_t index_offset) -> void
{
    m_index_offset = index_offset;
}

auto AssetLoader::set_position_offset(const uint32_t position_offset) -> void
{
    m_position_offset = position_offset;
}

auto AssetLoader::set_vertex_offset(const uint32_t vertex_offset) -> void
{
    m_vertex_offset = vertex_offset;
}

auto AssetLoader::set_material_offset(const uint32_t material_offset) -> void
{
    m_material_offset = material_offset;
}

auto AssetLoader::index_offset() const -> std::optional<uint32_t>
{
    return m_index_offset;
}

auto AssetLoader::position_offset() const -> std::optional<uint32_t>
{
    return m_position_offset;
}

auto AssetLoader::vertex_offset() const -> std::optional<uint32_t>
{
    return m_vertex_offset;
}

auto AssetLoader::material_offset() const -> std::optional<uint32_t>
{
    return m_material_offset;
}

auto AssetLoader::lazy_indices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            std::ignore = this;
            PRECOND(out.size() == indices_size_bytes());
            std::memcpy(out.data(), indices.data(), out.size());
        }   //
    };
}

auto AssetLoader::lazy_positions_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            std::ignore = this;
            PRECOND(out.size() == positions_size_bytes());
            std::memcpy(out.data(), positions.data(), out.size());
        }   //
    };
}

auto AssetLoader::lazy_vertices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            std::ignore = this;
            PRECOND(out.size() == vertices_size_bytes());
            std::memcpy(out.data(), vertices.data(), out.size());
        }   //
    };
}

auto AssetLoader::lazy_materials_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [](const std::span<std::byte>) static -> void {}
    };
}

auto AssetLoader::lazy_draw_commands_copy() const noexcept
    -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            PRECOND(out.size() == draw_command_size_bytes());

            const shaders::DrawCommand shader_draw_command{ draw_command() };

            std::memcpy(out.data(), &shader_draw_command, out.size());
        }   //
    };
}

auto AssetLoader::max_draw_count() const noexcept -> uint32_t
{
    return 1;
}

auto AssetLoader::draw_command() const noexcept -> shaders::DrawCommand
{
    return shaders::DrawCommand{
        .vertex_count   = indices.empty() ? positions.size() : indices.size(),
        .instance_count = 1,
        .primitive{
                   .index_offset =
                indices_size_bytes() == 0
                    ? std::numeric_limits<decltype(shaders::Primitive::index_offset)>::max()
                    : *m_index_offset,
                   .position_offset = *m_position_offset,
                   .vertex_offset   = vertices_size_bytes() == 0
                                 ? std::numeric_limits<
                                       decltype(shaders::Primitive::vertex_offset)>::max()
                                 : *m_vertex_offset,
                   .stride          = sizeof(ShaderVertex) / 4,
                   .color_offset    = offsetof(ShaderVertex, color) / 4 }
    };
}

}   // namespace demo
