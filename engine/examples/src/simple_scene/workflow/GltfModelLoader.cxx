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

GltfModelLoader::GltfModelLoader(const fastgltf::Asset& model)
    : m_indices_size_bytes{ indices.size() * sizeof(Index) },
      m_positions_size_bytes{ positions.size() * sizeof(glm::vec3) },
      m_vertices_size_bytes{ vertices.size() * sizeof(ShaderVertex) },
      m_materials_size_bytes{ 0 },
      m_transforms_size_bytes{ 0 },
      m_draw_command_count{ 1 },
      m_write_indices{ make_index_writer(model) },
      m_write_positions{ make_position_writer(model) },
      m_write_vertices{ make_vertex_writer(model) },
      m_write_materials{ make_material_writer(model) },
      m_write_transforms{ make_transform_writer(model) },
      m_make_draw_command_writer{ make_draw_command_writer_factory(model) }
{
}

auto GltfModelLoader::indices_size_bytes() const noexcept -> uint32_t
{
    return m_indices_size_bytes;
}

auto GltfModelLoader::positions_size_bytes() const noexcept -> uint32_t
{
    return m_positions_size_bytes;
}

auto GltfModelLoader::vertices_size_bytes() const noexcept -> uint32_t
{
    return m_vertices_size_bytes;
}

auto GltfModelLoader::materials_size_bytes() const noexcept -> uint32_t
{
    return m_materials_size_bytes;
}

auto GltfModelLoader::transforms_size_bytes() const noexcept -> uint32_t
{
    return m_transforms_size_bytes;
}

auto GltfModelLoader::draw_command_count() const noexcept -> uint32_t
{
    return m_draw_command_count;
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

auto GltfModelLoader::set_index_offset(const uint32_t index_offset) -> void
{
    m_offsets.index_offset = index_offset;
}

auto GltfModelLoader::set_position_offset(const uint32_t position_offset) -> void
{
    m_offsets.position_offset = position_offset;
}

auto GltfModelLoader::set_vertex_offset(const uint32_t vertex_offset) -> void
{
    m_offsets.vertex_offset = vertex_offset;
}

auto GltfModelLoader::set_material_offset(const uint32_t material_offset) -> void
{
    m_offsets.material_offset = material_offset;
}

auto GltfModelLoader::set_transform_offset(const uint32_t transform_offset) -> void
{
    m_offsets.transform_offset = transform_offset;
}

auto GltfModelLoader::index_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{ m_write_indices };
}

auto GltfModelLoader::position_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{ m_write_positions };
}

auto GltfModelLoader::vertex_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{ m_write_vertices };
}

auto GltfModelLoader::material_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{ m_write_materials };
}

auto GltfModelLoader::transform_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{ m_write_transforms };
}

auto GltfModelLoader::draw_command_writer() const noexcept
    -> kiln::gfx::renderer::LazyCopy
{
    return m_make_draw_command_writer(m_offsets);
}

[[nodiscard]]
auto GltfModelLoader::make_index_writer(const fastgltf::Asset&) -> Writer
{
    return Writer{
        [](const std::span<std::byte> out) -> void
        {
            PRECOND(out.size() == indices.size() * sizeof(decltype(indices)::value_type));
            std::memcpy(out.data(), indices.data(), out.size());
        },
    };
}

[[nodiscard]]
auto GltfModelLoader::make_position_writer(const fastgltf::Asset&) -> Writer
{
    return Writer{
        [](const std::span<std::byte> out) -> void
        {
            PRECOND(
                out.size() == positions.size() * sizeof(decltype(positions)::value_type)
            );
            std::memcpy(out.data(), positions.data(), out.size());
        },
    };
}

[[nodiscard]]
auto GltfModelLoader::make_vertex_writer(const fastgltf::Asset&) -> Writer
{
    return Writer{
        [](const std::span<std::byte> out) -> void
        {
            PRECOND(
                out.size() == vertices.size() * sizeof(decltype(vertices)::value_type)
            );
            std::memcpy(out.data(), vertices.data(), out.size());
        },
    };
}

[[nodiscard]]
auto GltfModelLoader::make_material_writer(const fastgltf::Asset&) -> Writer
{
    return Writer{
        [](const std::span<std::byte>) -> void {},
    };
}

[[nodiscard]]
auto GltfModelLoader::make_transform_writer(const fastgltf::Asset&) -> Writer
{
    return Writer{
        [](const std::span<std::byte>) -> void {},
    };
}

auto GltfModelLoader::make_draw_command_writer_factory(const fastgltf::Asset&)
    -> DrawCommandWriterFactory
{
    return DrawCommandWriterFactory{
        [disable_indices    = indices.empty(),
         disable_vertices   = vertices.empty(),
         disable_materials  = true,
         disable_transforms = true]   //
        (const Offsets& offsets) -> kiln::gfx::renderer::LazyCopy
        {
            return kiln::gfx::renderer::LazyCopy{
                [disable_indices,
                 disable_vertices,
                 disable_materials,
                 disable_transforms,
                 offsets](const std::span<std::byte> out) -> void
                {
                    const shaders::Primitive primitive{
                        .index_offset =
                            disable_indices
                                ? std::numeric_limits<
                                      decltype(shaders::Primitive::index_offset)>::max()
                                : *offsets.index_offset,
                        .position_offset = *offsets.position_offset,
                        .vertex_offset =
                            disable_vertices
                                ? std::numeric_limits<
                                      decltype(shaders::Primitive::vertex_offset)>::max()
                                : *offsets.vertex_offset,
                        .stride       = sizeof(ShaderVertex) / 4,
                        .color_offset = offsetof(ShaderVertex, color) / 4,
                        .material_index =
                            disable_materials
                                ? std::numeric_limits<
                                      decltype(shaders::Primitive::material_index)>::max()
                                : *offsets.material_offset,
                    };
                    const shaders::DrawCommand draw_command{
                        .vertex_count   = indices.empty() ? positions.size()
                                                          : indices.size(),
                        .instance_count = 1,
                        .transform_offset =
                            disable_transforms
                                ? std::numeric_limits<
                                      decltype(shaders::DrawCommand::transform_offset)>::max()
                                : *offsets.transform_offset,
                        .primitive = primitive,
                    };
                    PRECOND(out.size() == sizeof(draw_command));
                    std::memcpy(out.data(), &draw_command, out.size());
                }
            };
        }
    };
}

}   // namespace demo
