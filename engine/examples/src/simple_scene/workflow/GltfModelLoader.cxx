module;

#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory_resource>
#include <numeric>
#include <optional>
#include <span>
#include <vector>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include "kiln/util/contract_macros.hpp"

module examples.simple_scene.workflow.GltfModelLoader;

import kiln.util.contracts;
import kiln.util.Overloaded;

import examples.simple_scene.shaders;

namespace demo {

GltfModelLoader::GltfModelLoader(
    const GltfModelLoader& other,
    const allocator_type&  allocator
)
    : m_model{ other.m_model },
      m_manifest{ other.m_manifest, allocator },
      m_offsets{ other.m_offsets }
{
}

GltfModelLoader::GltfModelLoader(GltfModelLoader&& other, const allocator_type& allocator)
    : m_model{ other.m_model },
      m_manifest{ std::move(other.m_manifest), allocator },
      m_offsets{ other.m_offsets }
{
}

GltfModelLoader::GltfModelLoader(
    const fastgltf::Asset& model,
    const std::size_t      scene_index,
    const glm::mat4x4&     transform
)
    : GltfModelLoader{
          std::allocator_arg,
          std::pmr::get_default_resource(),   //
          model,
          scene_index,
          transform,
      }
{
}

GltfModelLoader::GltfModelLoader(
    std::allocator_arg_t,
    const allocator_type&  allocator,
    const fastgltf::Asset& model,
    const std::size_t      scene_index,
    const glm::mat4x4&     transform
)
    : m_model{ model },
      m_manifest{ std::allocator_arg, allocator, m_model, scene_index, transform }
{
}

auto GltfModelLoader::get_allocator() const noexcept -> allocator_type
{
    return m_manifest.get_allocator();
}

auto GltfModelLoader::geometry_buffer_size_bytes() const noexcept -> uint32_t
{
    return m_manifest.geometry_buffer_size_bytes();
}

auto GltfModelLoader::material_buffer_size_bytes() const noexcept -> uint32_t
{
    return m_manifest.material_buffer_size_bytes();
}

auto GltfModelLoader::instance_buffer_size_bytes() const noexcept -> uint32_t
{
    return m_manifest.instance_buffer_size_bytes();
}

auto GltfModelLoader::draw_command_count() const noexcept -> uint32_t
{
    return m_manifest.draw_command_count();
}

// ReSharper disable once CppDFAConstantFunctionResult
auto GltfModelLoader::geometry_buffer_alignment() noexcept -> uint32_t
{
    return Manifest::geometry_buffer_alignment();
}

// ReSharper disable once CppDFAConstantFunctionResult
auto GltfModelLoader::material_buffer_alignment() noexcept -> uint32_t
{
    return Manifest::material_buffer_alignment();
}

// ReSharper disable once CppDFAConstantFunctionResult
auto GltfModelLoader::instance_buffer_alignment() noexcept -> uint32_t
{
    return Manifest::instance_buffer_alignment();
}

auto GltfModelLoader::set_geometry_buffer_byte_offset(
    const uint32_t geometry_buffer_byte_offset
) -> void
{
    m_offsets.geometry_buffer_byte_offset = geometry_buffer_byte_offset;
}

auto GltfModelLoader::set_material_buffer_byte_offset(
    const uint32_t material_buffer_byte_offset
) -> void
{
    m_offsets.material_offset = material_buffer_byte_offset;
}

auto GltfModelLoader::set_instance_buffer_byte_offset(
    const uint32_t instance_buffer_byte_offset
) -> void
{
    m_offsets.instance_buffer_byte_offset = instance_buffer_byte_offset;
}

auto GltfModelLoader::geometry_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            m_manifest.write_geometry(m_model, out);   //
        },
    };
}

auto GltfModelLoader::material_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            m_manifest.write_materials(m_model, out);   //
        },
    };
}

auto GltfModelLoader::instance_writer() const noexcept -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            m_manifest.write_instances(m_model, out);   //
        },
    };
}

auto GltfModelLoader::draw_command_writer() const noexcept
    -> kiln::gfx::renderer::LazyCopy
{
    return kiln::gfx::renderer::LazyCopy{
        [this](const std::span<std::byte> out) -> void
        {
            m_manifest.write_draw_commands(m_model, m_offsets, out);   //
        }
    };
}

GltfModelLoader::Manifest::Manifest(const Manifest& other, const allocator_type& allocator)
    : m_geometry_buffer_size_bytes{ other.m_geometry_buffer_size_bytes },
      m_material_buffer_size_bytes{ other.m_material_buffer_size_bytes },
      m_draw_command_count{ other.m_draw_command_count },
      m_element_buffer_byte_offsets{ other.m_element_buffer_byte_offsets },
      m_accessor_element_byte_offsets{ other.m_accessor_element_byte_offsets, allocator },
      m_material_indices{ other.m_material_indices, allocator },
      m_per_mesh_instance_offsets{ other.m_per_mesh_instance_offsets, allocator },
      m_per_mesh_instance_counts{ other.m_per_mesh_instance_counts, allocator },
      m_geometry_writers{ other.m_geometry_writers, allocator },
      m_materials{ other.m_materials, allocator },
      m_transforms{ other.m_transforms, allocator },
      m_normal_matrices{ other.m_normal_matrices, allocator }
{
}

GltfModelLoader::Manifest::Manifest(Manifest&& other, const allocator_type& allocator)
    : m_geometry_buffer_size_bytes{ other.m_geometry_buffer_size_bytes },
      m_material_buffer_size_bytes{ other.m_material_buffer_size_bytes },
      m_draw_command_count{ other.m_draw_command_count },
      m_element_buffer_byte_offsets{ other.m_element_buffer_byte_offsets },
      m_accessor_element_byte_offsets{ std::move(other.m_accessor_element_byte_offsets),
                                       allocator },
      m_material_indices{ std::move(other.m_material_indices), allocator },
      m_per_mesh_instance_offsets{ std::move(other.m_per_mesh_instance_offsets),
                                   allocator },
      m_per_mesh_instance_counts{ std::move(other.m_per_mesh_instance_counts),
                                  allocator },
      m_geometry_writers{ std::move(other.m_geometry_writers), allocator },
      m_materials{ std::move(other.m_materials), allocator },
      m_transforms{ std::move(other.m_transforms), allocator },
      m_normal_matrices{ std::move(other.m_normal_matrices), allocator }
{
}

GltfModelLoader::Manifest::Manifest(
    const fastgltf::Asset& model,
    const std::size_t      scene_index,
    const glm::mat4x4&     transform
)
    : Manifest{
          std::allocator_arg,
          std::pmr::get_default_resource(),   //
          model,
          scene_index,
          transform,
      }
{
}

GltfModelLoader::Manifest::Manifest(
    std::allocator_arg_t,
    const allocator_type&  allocator,
    const fastgltf::Asset& model,
    const std::size_t      scene_index,
    const glm::mat4x4&     transform
)
    : m_accessor_element_byte_offsets{ allocator },
      m_material_indices{ allocator },
      m_per_mesh_instance_offsets{ allocator },
      m_per_mesh_instance_counts{ allocator },
      m_geometry_writers{ allocator },
      m_materials{ allocator },
      m_transforms{ allocator },
      m_normal_matrices{ allocator }
{
    preprocess(model, scene_index, *this);

    for (const std::size_t node_index : model.scenes[scene_index].nodeIndices)
    {
        process(model, transform, model.nodes[node_index], *this);
    }

    m_geometry_buffer_size_bytes = std::reduce(
        m_element_buffer_byte_offsets.begin(),
        m_element_buffer_byte_offsets.end()
    );
    std::exclusive_scan(
        m_element_buffer_byte_offsets.begin(),
        m_element_buffer_byte_offsets.end(),
        m_element_buffer_byte_offsets.begin(),
        0
    );

    assert(m_normal_matrices.empty());
    assert(m_normal_matrices.capacity() == m_transforms.size());
    for (glm::mat4x4& instance_transform : m_transforms)
    {
        const glm::mat4x4 vulkan_basis_correction{
            glm::scale(glm::identity<glm::mat4x4>(), glm::vec3{ 1, -1, -1 })
        };

        instance_transform
            = vulkan_basis_correction * instance_transform * vulkan_basis_correction;

        const glm::mat3x3 orientation_matrix{ instance_transform };

        m_normal_matrices.push_back(
            glm::transpose(glm::inverse(orientation_matrix))
            * (glm::determinant(orientation_matrix) < 0 ? -1.f : 1.f)
        );
    }
}

auto GltfModelLoader::Manifest::get_allocator() const noexcept -> allocator_type
{
    return m_geometry_writers.get_allocator();
}

auto GltfModelLoader::Manifest::geometry_buffer_size_bytes() const noexcept -> uint32_t
{
    return m_geometry_buffer_size_bytes;
}

auto GltfModelLoader::Manifest::material_buffer_size_bytes() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(
        m_materials.size() * sizeof(decltype(m_materials)::value_type)
    );
}

auto GltfModelLoader::Manifest::instance_buffer_size_bytes() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(
        m_transforms.size() * sizeof(decltype(m_transforms)::value_type)
        + m_normal_matrices.size() * sizeof(decltype(m_normal_matrices)::value_type)
    );
}

auto GltfModelLoader::Manifest::draw_command_count() const noexcept -> uint32_t
{
    return m_draw_command_count;
}

auto GltfModelLoader::Manifest::geometry_buffer_alignment() noexcept -> uint32_t
{
    return std::max(alignof(shaders::Index), alignof(float));
}

auto GltfModelLoader::Manifest::material_buffer_alignment() noexcept -> uint32_t
{
    return alignof(decltype(m_materials)::value_type);
}

auto GltfModelLoader::Manifest::instance_buffer_alignment() noexcept -> uint32_t
{
    return std::max(
        alignof(decltype(m_transforms)::value_type),
        alignof(decltype(m_normal_matrices)::value_type)
    );
}

auto GltfModelLoader::Manifest::write_geometry(
    const fastgltf::Asset&     model,
    const std::span<std::byte> out
) const -> void
{
    PRECOND(out.size() == geometry_buffer_size_bytes());
    PRECOND(
        reinterpret_cast<std::uintptr_t>(out.data()) % geometry_buffer_alignment() == 0
    );

    for (const Writer& geometry_writer : m_geometry_writers)
    {
        geometry_writer(model, m_element_buffer_byte_offsets, out);
    }
}

auto GltfModelLoader::Manifest::write_materials(
    const fastgltf::Asset&,
    const std::span<std::byte> out
) const -> void
{
    PRECOND(out.size() == material_buffer_size_bytes());
    PRECOND(
        reinterpret_cast<std::uintptr_t>(out.data()) % material_buffer_alignment() == 0
    );

    std::memcpy(
        out.data(),
        m_materials.data(),
        m_materials.size() * sizeof(decltype(m_materials)::value_type)
    );
}

auto GltfModelLoader::Manifest::write_instances(
    const fastgltf::Asset&,
    const std::span<std::byte> out
) const -> void
{
    PRECOND(out.size() == instance_buffer_size_bytes());
    PRECOND(
        reinterpret_cast<std::uintptr_t>(out.data()) % instance_buffer_alignment() == 0
    );

    std::memcpy(
        out.data(),
        m_transforms.data(),
        m_transforms.size() * sizeof(decltype(m_transforms)::value_type)
    );

    const std::size_t normal_matrix_buffer_byte_offset{
        m_transforms.size() * sizeof(decltype(m_transforms)::value_type)
    };
    std::memcpy(
        out.data() + normal_matrix_buffer_byte_offset,
        m_normal_matrices.data(),
        m_normal_matrices.size() * sizeof(decltype(m_normal_matrices)::value_type)
    );
}

auto GltfModelLoader::Manifest::write_draw_commands(
    const fastgltf::Asset&     model,
    const Offsets&             global_offsets,
    const std::span<std::byte> out
) const -> void
{
    PRECOND(out.size() == m_draw_command_count * sizeof(shaders::DrawCommand));
    PRECOND(
        reinterpret_cast<std::uintptr_t>(out.data()) % alignof(shaders::DrawCommand) == 0
    );

    const std::span out_draw_commands{
        reinterpret_cast<shaders::DrawCommand*>(out.data()),
        m_draw_command_count,
    };

    auto out_iter{ out_draw_commands.begin() };
    for (std::size_t mesh_index{}; mesh_index < model.meshes.size(); ++mesh_index)
    {
        if (m_per_mesh_instance_counts[mesh_index] > 0)
        {
            for (const fastgltf::Primitive& primitive :
                 model.meshes[mesh_index].primitives)
            {
                *out_iter = shaders::DrawCommand{
                    .vertex_count = static_cast<uint32_t>(
                        primitive.indicesAccessor.has_value()
                            ? model.accessors[*primitive.indicesAccessor].count
                            : model.accessors[primitive.attributes.front().accessorIndex]
                                  .count
                    ),
                    .instance_count = m_per_mesh_instance_counts[mesh_index],
                    .first_vertex   = 0,
                    .first_instance = 0,
                    .primitive      = shader_primitive_from(global_offsets, primitive),
                    .transform_buffer_byte_offset
                    = *global_offsets.instance_buffer_byte_offset
                    + m_per_mesh_instance_offsets[mesh_index]
                          * static_cast<uint32_t>(
                              sizeof(decltype(m_transforms)::value_type)
                          ),
                    .normal_matrix_buffer_byte_offset
                    = *global_offsets.instance_buffer_byte_offset
                    + static_cast<uint32_t>(
                        m_transforms.size() * sizeof(decltype(m_transforms)::value_type)
                    )
                    + m_per_mesh_instance_offsets[mesh_index]
                          * static_cast<uint32_t>(
                              sizeof(decltype(m_normal_matrices)::value_type)
                          ),
                };
                ++out_iter;
            }
        }
    }
}

[[nodiscard]]
auto is_supported_attribute(const std::string_view attribute_name) -> bool
{
    using std::string_view_literals::operator""sv;
    constexpr static std::array supported_attribute_names{
        "POSITION"sv,   "NORMAL"sv,     "TANGENT"sv,
        "TEXCOORD_0"sv, "TEXCOORD_1"sv, "COLOR_0"sv,
    };

    return std::ranges::contains(supported_attribute_names, attribute_name);
}

[[nodiscard]]
auto texture_index_from(const auto& optional_texture_info) -> uint32_t
{
    if (!optional_texture_info.has_value())
    {
        return std::numeric_limits<uint32_t>::max();
    }
    return static_cast<uint32_t>(optional_texture_info->textureIndex);
}

[[nodiscard]]
auto texture_uv_index_from(const auto& optional_texture_info) -> uint32_t
{
    if (!optional_texture_info.has_value())
    {
        return std::numeric_limits<uint32_t>::max();
    }
    return static_cast<uint32_t>(optional_texture_info->texCoordIndex);
}

[[nodiscard]]
auto scale_from(const fastgltf::Optional<fastgltf::NormalTextureInfo>& texture_info)
    -> float
{
    if (!texture_info.has_value())
    {
        return 1.f;
    }
    return texture_info->scale;
}

[[nodiscard]]
auto strength_from(const fastgltf::Optional<fastgltf::OcclusionTextureInfo>& texture_info)
    -> float
{
    if (!texture_info.has_value())
    {
        return 1.f;
    }
    return texture_info->strength;
}

[[nodiscard]]
auto material_from(const fastgltf::Material& material) -> shaders::Material
{
    return shaders::Material{
        .base_color_factor = glm::make_vec4(material.pbrData.baseColorFactor.data()),
        .base_color_texture_index = texture_index_from(material.pbrData.baseColorTexture),
        .base_color_texture_uv_index
        = texture_uv_index_from(material.pbrData.baseColorTexture),
        .metallic_factor  = material.pbrData.metallicFactor,
        .roughness_factor = material.pbrData.roughnessFactor,
        .metallic_roughness_texture_index
        = texture_index_from(material.pbrData.metallicRoughnessTexture),
        .metallic_roughness_texture_uv_index
        = texture_uv_index_from(material.pbrData.metallicRoughnessTexture),
        .normal_texture_index       = texture_index_from(material.normalTexture),
        .normal_texture_uv_index    = texture_uv_index_from(material.normalTexture),
        .normal_texture_scale       = scale_from(material.normalTexture),
        .occlusion_texture_index    = texture_index_from(material.occlusionTexture),
        .occlusion_texture_uv_index = texture_uv_index_from(material.occlusionTexture),
        .occlusion_texture_strength = strength_from(material.occlusionTexture),
        .emissive_texture_index     = texture_index_from(material.emissiveTexture),
        .emissive_texture_uv_index  = texture_uv_index_from(material.emissiveTexture),
        .emissive_factor            = glm::make_vec3(material.emissiveFactor.data()),
        .alpha_cutoff
        = material.alphaMode == fastgltf::AlphaMode::Opaque ? -1.f : material.alphaCutoff,
    };
}

auto GltfModelLoader::Manifest::preprocess(
    const fastgltf::Asset& model,
    const std::size_t      scene_index,
    Manifest&              manifest
) -> void
{
    manifest.m_accessor_element_byte_offsets.resize(
        model.accessors.size(),
        std::numeric_limits<
            decltype(manifest.m_accessor_element_byte_offsets)::value_type>::max()
    );
    manifest.m_material_indices
        .resize(model.materials.size(), std::numeric_limits<uint32_t>::max());
    manifest.m_per_mesh_instance_offsets.resize(model.meshes.size(), 0);
    manifest.m_per_mesh_instance_counts.resize(model.meshes.size(), 0);

    uint32_t geometry_writer_count{};
    uint32_t material_count{};
    uint32_t instance_count{};

    const auto increment_counts{
        [&](this const auto& self, const fastgltf::Node& node) -> void
        {
            if (node.meshIndex.has_value())
            {
                for (const fastgltf::Primitive& primitive :
                     model.meshes[*node.meshIndex].primitives)
                {
                    if (primitive.indicesAccessor.has_value())
                    {
                        ++geometry_writer_count;
                    }

                    for (const auto& [attribute_name, _] : primitive.attributes)
                    {
                        if (is_supported_attribute(attribute_name))
                        {
                            ++geometry_writer_count;
                        }
                    }

                    if (primitive.materialIndex.has_value()
                        && manifest.m_material_indices[*primitive.materialIndex]
                               == std::numeric_limits<uint32_t>::max())
                    {
                        manifest.m_material_indices[*primitive.materialIndex]
                            = material_count;
                        ++material_count;
                    }
                }

                ++instance_count;

                ++manifest.m_per_mesh_instance_offsets[*node.meshIndex];
            }

            for (const auto child_node_index : node.children)
            {
                self(model.nodes[child_node_index]);
            }
        },
    };

    for (const auto node_index : model.scenes[scene_index].nodeIndices)
    {
        increment_counts(model.nodes[node_index]);
    }

    manifest.m_geometry_writers.reserve(geometry_writer_count);
    manifest.m_materials.resize(material_count);
    manifest.m_transforms.resize(instance_count, glm::identity<glm::mat4x4>());
    manifest.m_normal_matrices.reserve(instance_count);

    for (uint32_t original_material_index{};
         original_material_index < model.materials.size();
         ++original_material_index)
    {
        if (const uint32_t material_index{
                manifest.m_material_indices[original_material_index] };
            material_index != std::numeric_limits<uint32_t>::max())
        {
            manifest.m_materials[material_index]
                = material_from(model.materials[original_material_index]);
        }
    }

    std::exclusive_scan(
        manifest.m_per_mesh_instance_offsets.begin(),
        manifest.m_per_mesh_instance_offsets.end(),
        manifest.m_per_mesh_instance_offsets.begin(),
        0
    );
}

[[nodiscard]]
auto transform_matrix_from(
    const std::variant<fastgltf::TRS, fastgltf::math::fmat4x4>& transform
) -> glm::mat4x4
{
    return std::visit(
        kiln::util::Overloaded{
            [](const fastgltf::TRS& trs) -> glm::mat4x4
            {
                return glm::translate(
                           glm::identity<glm::mat4x4>(),
                           glm::make_vec3(trs.translation.data())
                       )
                     * glm::mat4_cast(glm::make_quat(trs.rotation.data()))
                     * glm::scale(
                           glm::identity<glm::mat4x4>(),
                           glm::make_vec3(trs.scale.data())
                     );
            },
            [](const fastgltf::math::fmat4x4& matrix) -> glm::mat4x4
            {
                return glm::make_mat4(matrix.data());   //
            },
        },
        transform
    );
}

auto GltfModelLoader::Manifest::process(
    const fastgltf::Asset& model,
    const glm::mat4x4&     transform,
    const fastgltf::Node&  node,
    Manifest&              manifest
) -> void
{
    const glm::mat4x4 global_transform{
        transform * transform_matrix_from(node.transform),
    };

    if (node.meshIndex.has_value())
    {
        process(model, global_transform, *node.meshIndex, manifest);
    }

    for (const std::size_t child_node_index : node.children)
    {
        process(model, global_transform, model.nodes[child_node_index], manifest);
    }
}

auto GltfModelLoader::Manifest::process(
    const fastgltf::Asset& model,
    const glm::mat4x4&     transform,
    const std::size_t      mesh_index,
    Manifest&              manifest
) -> void
{
    for (const fastgltf::Primitive& primitive : model.meshes[mesh_index].primitives)
    {
        process(model, primitive, manifest);
    }

    manifest.m_transforms
        [manifest.m_per_mesh_instance_offsets[mesh_index]
         + manifest.m_per_mesh_instance_counts[mesh_index]] = transform;
    if (manifest.m_per_mesh_instance_counts[mesh_index] == 0)
    {
        manifest.m_draw_command_count
            += static_cast<uint32_t>(model.meshes[mesh_index].primitives.size());
    }
    ++manifest.m_per_mesh_instance_counts[mesh_index];
}

[[nodiscard]]
auto vertex_element_size_from(const fastgltf::Attribute& attribute) -> uint32_t
{
    if (attribute.name == "NORMAL")
    {
        return sizeof(glm::vec3);
    }
    if (attribute.name == "TANGENT")
    {
        return sizeof(glm::vec4);
    }
    if (attribute.name == "TEXCOORD_0")
    {
        return sizeof(glm::vec2);
    }
    if (attribute.name == "TEXCOORD_1")
    {
        return sizeof(glm::vec2);
    }
    if (attribute.name == "COLOR_0")
    {
        return sizeof(glm::vec4);
    }
    return 0;
}

[[nodiscard]]
auto vertex_stride_from(const fastgltf::Primitive& primitive) -> uint32_t
{
    uint32_t stride{};

    for (const fastgltf::Attribute& attribute : primitive.attributes)
    {
        stride += vertex_element_size_from(attribute);
    }

    return stride;
}

[[nodiscard]]
auto supported_element_type_from(const std::string_view attribute_name)
    -> std::optional<SupportedElementType>
{
    if (attribute_name == "POSITION")
    {
        return SupportedElementType::ePosition;
    }
    if (attribute_name == "NORMAL")
    {
        return SupportedElementType::eNormal;
    }
    if (attribute_name == "TANGENT")
    {
        return SupportedElementType::eTangent;
    }
    if (attribute_name == "TEXCOORD_0")
    {
        return SupportedElementType::eTexCoord0;
    }
    if (attribute_name == "TEXCOORD_1")
    {
        return SupportedElementType::eTexCoord1;
    }
    if (attribute_name == "COLOR_0")
    {
        return SupportedElementType::eColor0;
    }
    return std::nullopt;
}

// ReSharper disable once CppNotAllPathsReturnValue
[[nodiscard]]
auto element_size_from(const SupportedElementType attribute_type) -> uint32_t
{
    // NOLINTBEGIN(bugprone-branch-clone)
    switch (attribute_type)
    {
        case SupportedElementType::eIndex:     return sizeof(uint32_t);
        case SupportedElementType::ePosition:  return sizeof(glm::vec3);
        case SupportedElementType::eNormal:    return sizeof(glm::vec3);
        case SupportedElementType::eTangent:   return sizeof(glm::vec4);
        case SupportedElementType::eTexCoord0: return sizeof(glm::vec2);
        case SupportedElementType::eTexCoord1: return sizeof(glm::vec2);
        case SupportedElementType::eColor0:    return sizeof(glm::vec4);
        case SupportedElementType::COUNT:      std::unreachable();
    }
    // NOLINTEND(bugprone-branch-clone)
}

auto GltfModelLoader::Manifest::process(
    const fastgltf::Asset&     model,
    const fastgltf::Primitive& primitive,
    Manifest&                  manifest
) -> void
{
    if (primitive.type != fastgltf::PrimitiveType::Triangles)
    {
        throw std::runtime_error{ "Only triangles are supported" };
    }

    if (!primitive.indicesAccessor.has_value())
    {
        throw std::runtime_error{ "Non-indexed geometry is not supported" };
    }
    if (primitive.indicesAccessor.has_value()
        && manifest.m_accessor_element_byte_offsets[*primitive.indicesAccessor]
               == std::numeric_limits<uint32_t>::max())
    {
        constexpr static auto element_type_index{
            std::to_underlying(SupportedElementType::eIndex)
        };

        manifest.m_accessor_element_byte_offsets[*primitive.indicesAccessor]
            = manifest.m_element_buffer_byte_offsets[element_type_index];
        manifest.m_geometry_writers.push_back(
            element_writer_from<shaders::Index>(
                SupportedElementType::eIndex,
                *primitive.indicesAccessor,
                manifest.m_element_buffer_byte_offsets[element_type_index]
            )
        );
        manifest.m_element_buffer_byte_offsets[element_type_index]
            += model.accessors[*primitive.indicesAccessor].count * sizeof(shaders::Index);
    }

    for (const auto& [attribute_name, accessor_index] : primitive.attributes)
    {
        if (manifest.m_accessor_element_byte_offsets[accessor_index]
            == std::numeric_limits<uint32_t>::max())
        {
            if (const std::optional<SupportedElementType> element_type{
                    supported_element_type_from(attribute_name) };
                element_type.has_value())
            {
                const auto element_type_index{ std::to_underlying(*element_type) };

                manifest.m_accessor_element_byte_offsets[accessor_index]
                    = manifest.m_element_buffer_byte_offsets[element_type_index];
                manifest.m_geometry_writers.push_back(element_writer_from(
                    *element_type,
                    model.accessors[accessor_index].type,
                    accessor_index,
                    manifest.m_element_buffer_byte_offsets[element_type_index]
                ));
                manifest.m_element_buffer_byte_offsets[element_type_index]
                    += model.accessors[accessor_index].count
                     * element_size_from(*element_type);
            }
        }
    }
}

[[nodiscard]]
auto parse_buffer(const fastgltf::Asset& asset, const std::size_t buffer_view_index)
    -> std::span<const std::byte>
{
    const fastgltf::BufferView& buffer_view{ asset.bufferViews[buffer_view_index] };
    const fastgltf::Buffer&     buffer{ asset.buffers[buffer_view.bufferIndex] };

    const auto data = std::visit(
        kiln::util::Overloaded{
            [] [[noreturn]] (auto&&) -> std::span<const std::byte>
            {
                std::unreachable();   //
            },
            [](const fastgltf::sources::URI&) -> std::span<const std::byte>
            {
                PRECOND(false, "External buffers must be preloaded");
                std::unreachable();
            },
            [](const fastgltf::sources::Array& array) -> std::span<const std::byte>
            {
                return std::span{ array.bytes.data(), array.bytes.size_bytes() };   //
            },
        },
        buffer.data
    );

    return data.subspan(buffer_view.byteOffset, buffer_view.byteLength);
}

template <typename Element_T>
auto GltfModelLoader::Manifest::element_writer_from(
    const SupportedElementType element_type,
    const std::size_t          accessor_index,
    const uint32_t             element_byte_offset
) -> Writer
{
    return Writer{
        [element_type, accessor_index, element_byte_offset](
            const fastgltf::Asset& model,
            const std::span<const uint32_t, std::to_underlying(SupportedElementType::COUNT)>
                                       element_buffer_byte_offsets,
            const std::span<std::byte> out
        ) -> void
        {
            assert(
                out.subspan(element_byte_offset).size()
                >= model.accessors[accessor_index].count * sizeof(Element_T)
            );
            assert(
                reinterpret_cast<std::uintptr_t>(out.subspan(element_byte_offset).data())
                    % alignof(Element_T)
                == 0
            );

            // Reverse index winding to adjust for flipped Y and Z axis
            if (element_type == SupportedElementType::eIndex)
            {
                const std::span index_out{
                    reinterpret_cast<shaders::Index*>(
                        out.data()
                        + element_buffer_byte_offsets[std::to_underlying(element_type)]
                        + element_byte_offset
                    ),
                    model.accessors[accessor_index].count,
                };

                fastgltf::iterateAccessorWithIndex<shaders::Index>(
                    model,
                    model.accessors[accessor_index],
                    [index_out](const shaders::Index index_val, const std::size_t index)
                        -> void
                    {
                        const std::size_t triangle_component = index % 3;
                        const std::size_t triangle_start     = index - triangle_component;

                        if (triangle_component == 1)
                        {
                            index_out[triangle_start + 2] = index_val;
                        }
                        else if (triangle_component == 2)
                        {
                            index_out[triangle_start + 1] = index_val;
                        }
                        else
                        {
                            index_out[index] = index_val;
                        }
                    },
                    parse_buffer
                );
            }
            // Flip Y and Z axis to match Vulkan's coordinate system
            else if (element_type == SupportedElementType::ePosition
                     || element_type == SupportedElementType::eNormal)
            {
                const std::span vec3_out{
                    reinterpret_cast<glm::vec3*>(
                        out.data()
                        + element_buffer_byte_offsets[std::to_underlying(element_type)]
                        + element_byte_offset
                    ),
                    model.accessors[accessor_index].count,
                };

                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    model,
                    model.accessors[accessor_index],
                    [vec3_out](const glm::vec3 value, const std::size_t index) -> void
                    {
                        vec3_out[index] = glm::vec3{ value.x, -value.y, -value.z };   //
                    },
                    parse_buffer
                );
            }
            // Flip Y and Z axis to match Vulkan's coordinate system
            else if (element_type == SupportedElementType::eTangent)
            {
                const std::span vec4_out{
                    reinterpret_cast<glm::vec4*>(
                        out.data()
                        + element_buffer_byte_offsets[std::to_underlying(element_type)]
                        + element_byte_offset
                    ),
                    model.accessors[accessor_index].count,
                };

                fastgltf::iterateAccessorWithIndex<glm::vec4>(
                    model,
                    model.accessors[accessor_index],
                    [vec4_out](const glm::vec4 value, const std::size_t index) -> void
                    {
                        vec4_out[index]
                            = glm::vec4{ value.x, -value.y, -value.z, value.w };
                    },
                    parse_buffer
                );
            }
            else
            {
                fastgltf::copyFromAccessor<Element_T>(
                    model,
                    model.accessors[accessor_index],
                    out.data()
                        + element_buffer_byte_offsets[std::to_underlying(element_type)]
                        + element_byte_offset,
                    parse_buffer
                );
            }
        },
    };
}

// ReSharper disable once CppNotAllPathsReturnValue
auto GltfModelLoader::Manifest::element_writer_from(
    const SupportedElementType   element_type,
    const fastgltf::AccessorType accessor_type,
    const std::size_t            accessor_index,
    const uint32_t               element_byte_offset
) -> Writer
{
    // NOLINTBEGIN(bugprone-branch-clone)
    switch (element_type)
    {
        case SupportedElementType::eIndex:
            return element_writer_from<shaders::Index>(
                element_type,
                accessor_index,
                element_byte_offset
            );
        case SupportedElementType::ePosition:
            return element_writer_from<glm::vec3>(
                element_type,
                accessor_index,
                element_byte_offset
            );
        case SupportedElementType::eNormal:
            return element_writer_from<glm::vec3>(
                element_type,
                accessor_index,
                element_byte_offset
            );
        case SupportedElementType::eTangent:
            return element_writer_from<glm::vec4>(
                element_type,
                accessor_index,
                element_byte_offset
            );
        case SupportedElementType::eTexCoord0:
            return element_writer_from<glm::vec2>(
                element_type,
                accessor_index,
                element_byte_offset
            );
        case SupportedElementType::eTexCoord1:
            return element_writer_from<glm::vec2>(
                element_type,
                accessor_index,
                element_byte_offset
            );
        case SupportedElementType::eColor0:
            if (accessor_type == fastgltf::AccessorType::Vec3)
            {
                return color_vec3_writer_from(
                    element_type,
                    accessor_index,
                    element_byte_offset
                );
            }
            return element_writer_from<glm::vec4>(
                element_type,
                accessor_index,
                element_byte_offset
            );
        case SupportedElementType::COUNT: std::unreachable();
    }
    // NOLINTEND(bugprone-branch-clone)
}

[[nodiscard]]
auto element_type_represents_color(const SupportedElementType element_type) noexcept
    -> bool
{
    return element_type == SupportedElementType::eColor0;
}

auto GltfModelLoader::Manifest::color_vec3_writer_from(
    const SupportedElementType element_type,
    const std::size_t          accessor_index,
    const uint32_t             element_byte_offset
) -> Writer
{
    PRECOND(element_type_represents_color(element_type));

    return Writer{
        [element_type, accessor_index, element_byte_offset](
            const fastgltf::Asset& model,
            const std::span<const uint32_t, std::to_underlying(SupportedElementType::COUNT)>
                                       element_buffer_byte_offsets,
            const std::span<std::byte> out
        ) -> void
        {
            assert(
                out.subspan(element_byte_offset).size()
                >= model.accessors[accessor_index].count * sizeof(glm::vec4)
            );
            assert(
                reinterpret_cast<std::uintptr_t>(out.subspan(element_byte_offset).data())
                    % alignof(glm::vec4)
                == 0
            );

            const std::span color_out{
                reinterpret_cast<glm::vec4*>(
                    out.data()
                    + element_buffer_byte_offsets[std::to_underlying(element_type)]
                    + element_byte_offset
                ),
                model.accessors[accessor_index].count,
            };

            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                model,
                model.accessors[accessor_index],
                [color_out](const glm::vec3 color, const std::size_t index) -> void
                {
                    color_out[index] = glm::vec4{ color, 1 };   //
                },
                parse_buffer
            );
        },
    };
}

auto GltfModelLoader::Manifest::shader_primitive_from(
    const Offsets&             global_offsets,
    const fastgltf::Primitive& primitive
) const -> shaders::Primitive
{
    shaders::Primitive shader_primitive{};

    if (primitive.indicesAccessor.has_value())
    {
        shader_primitive.index_buffer_byte_offset
            = *global_offsets.geometry_buffer_byte_offset
            + m_element_buffer_byte_offsets[std::to_underlying(SupportedElementType::eIndex)]
            + m_accessor_element_byte_offsets[*primitive.indicesAccessor];
    }

    for (const auto& [attribute_name, accessor_index] : primitive.attributes)
    {
        if (attribute_name == "POSITION")
        {
            shader_primitive.position_buffer_byte_offset
                = *global_offsets.geometry_buffer_byte_offset
                + m_element_buffer_byte_offsets   //
                      [std::to_underlying(SupportedElementType::ePosition)]
                + m_accessor_element_byte_offsets[accessor_index];
        }
        else if (attribute_name == "NORMAL")
        {
            shader_primitive.normal_buffer_byte_offset
                = *global_offsets.geometry_buffer_byte_offset
                + m_element_buffer_byte_offsets   //
                      [std::to_underlying(SupportedElementType::eNormal)]
                + m_accessor_element_byte_offsets[accessor_index];
        }
        else if (attribute_name == "TANGENT")
        {
            shader_primitive.tangent_buffer_byte_offset
                = *global_offsets.geometry_buffer_byte_offset
                + m_element_buffer_byte_offsets   //
                      [std::to_underlying(SupportedElementType::eTangent)]
                + m_accessor_element_byte_offsets[accessor_index];
        }
        else if (attribute_name == "TEXCOORD_0")
        {
            shader_primitive.uv_0_buffer_byte_offset
                = *global_offsets.geometry_buffer_byte_offset
                + m_element_buffer_byte_offsets   //
                      [std::to_underlying(SupportedElementType::eTexCoord0)]
                + m_accessor_element_byte_offsets[accessor_index];
        }
        else if (attribute_name == "TEXCOORD_1")
        {
            shader_primitive.uv_1_buffer_byte_offset
                = *global_offsets.geometry_buffer_byte_offset
                + m_element_buffer_byte_offsets   //
                      [std::to_underlying(SupportedElementType::eTexCoord1)]
                + m_accessor_element_byte_offsets[accessor_index];
        }
        else if (attribute_name == "COLOR_0")
        {
            shader_primitive.color_buffer_byte_offset
                = *global_offsets.geometry_buffer_byte_offset
                + m_element_buffer_byte_offsets   //
                      [std::to_underlying(SupportedElementType::eColor0)]
                + m_accessor_element_byte_offsets[accessor_index];
        }
        else
        {
            assert(!is_supported_attribute(attribute_name));
        }
    }

    if (primitive.materialIndex.has_value())
    {
        assert(
            m_material_indices[*primitive.materialIndex]
            != std::numeric_limits<uint32_t>::max()
        );
        shader_primitive.material_index = *global_offsets.material_offset
                                        + m_material_indices[*primitive.materialIndex];
    }

    return shader_primitive;
}

auto GltfModelLoader::Manifest::vertex_offset_from(
    const fastgltf::Primitive& primitive
) const -> uint32_t
{
    uint32_t result{ std::numeric_limits<uint32_t>::max() };

    for (const auto& [_, accessor_index] : primitive.attributes)
    {
        if (const auto accessor_offset{ m_accessor_element_byte_offsets[accessor_index] };
            accessor_offset < result)
        {
            result = accessor_offset;
        }
    }

    return result;
}

}   // namespace demo
