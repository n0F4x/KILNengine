module;

#include <array>
#include <cstddef>
#include <limits>
#include <span>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <fastgltf/core.hpp>

#include "kiln/util/contract_macros.hpp"

module examples.simple_scene.workflow.GltfModelLoader;

import kiln.util.contracts;

import examples.simple_scene.shaders;

namespace demo {

GltfModelLoader::GltfModelLoader(const fastgltf::Asset& model) : m_model{ model } {}

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

auto GltfModelLoader::indices_size_bytes() const noexcept -> uint32_t
{
    return indices.size() * sizeof(Index);
}

auto GltfModelLoader::positions_size_bytes() const noexcept -> uint32_t
{
    return positions.size() * sizeof(glm::vec3);
}

auto GltfModelLoader::vertices_size_bytes() const noexcept -> uint32_t
{
    return vertices.size() * sizeof(ShaderVertex);
}

auto GltfModelLoader::materials_size_bytes() const noexcept -> uint32_t
{
    return 0;
}

auto GltfModelLoader::transforms_size_bytes() const noexcept -> uint32_t
{
    return 0;
}

auto GltfModelLoader::draw_command_size_bytes() const noexcept -> uint32_t
{
    return sizeof(shaders::DrawCommand);
}

auto GltfModelLoader::index_alignment() const noexcept -> uint32_t
{
    return alignof(Index);
}

auto GltfModelLoader::position_alignment() const noexcept -> uint32_t
{
    return alignof(glm::vec3);
}

auto GltfModelLoader::vertex_alignment() const noexcept -> uint32_t
{
    return alignof(ShaderVertex);
}

auto GltfModelLoader::material_alignment() const noexcept -> uint32_t
{
    return alignof(shaders::Material);
}

auto GltfModelLoader::transform_alignment() const noexcept -> uint32_t
{
    return alignof(glm::mat4x4);
}

auto GltfModelLoader::draw_command_alignment() const noexcept -> uint32_t
{
    return alignof(shaders::DrawCommand);
}

auto GltfModelLoader::set_index_offset(const uint32_t index_offset) -> void
{
    m_index_offset = index_offset;
}

auto GltfModelLoader::set_position_offset(const uint32_t position_offset) -> void
{
    m_position_offset = position_offset;
}

auto GltfModelLoader::set_vertex_offset(const uint32_t vertex_offset) -> void
{
    m_vertex_offset = vertex_offset;
}

auto GltfModelLoader::set_material_offset(const uint32_t material_offset) -> void
{
    m_material_offset = material_offset;
}

auto GltfModelLoader::set_transform_offset(const uint32_t transform_offset) -> void
{
    m_transform_offset = transform_offset;
}

auto GltfModelLoader::lazy_indices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
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

auto GltfModelLoader::lazy_positions_copy() const noexcept
    -> kiln::gfx::renderer::LazyCopy
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

auto GltfModelLoader::lazy_vertices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy
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

auto GltfModelLoader::lazy_materials_copy() const noexcept
    -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [](const std::span<std::byte>) static -> void {}
    };
}

auto GltfModelLoader::lazy_transforms_copy() const noexcept
    -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [](const std::span<std::byte>) static -> void {}
    };
}

auto GltfModelLoader::lazy_draw_commands_copy() const noexcept
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

auto GltfModelLoader::max_draw_count() const noexcept -> uint32_t
{
    return 1;
}

auto GltfModelLoader::draw_command() const noexcept -> shaders::DrawCommand
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
