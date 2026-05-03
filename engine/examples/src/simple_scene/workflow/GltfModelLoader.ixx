module;

#include <array>
#include <functional>
#include <optional>
#include <vector>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_common.hpp>

#include <fastgltf/core.hpp>

#include "kiln/util/lifetimebound.hpp"

export module examples.simple_scene.workflow.GltfModelLoader;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.memory.LazyCopy;
import kiln.util.containers.CopyableFunction;

import examples.simple_scene.shaders;

namespace demo {

enum struct SupportedElementType : uint32_t
{
    eIndex,
    ePosition,
    eNormal,
    eTangent,
    eTexCoord0,
    eTexCoord1,
    eColor0,
    COUNT,
};

export class GltfModelLoader {
public:
    explicit GltfModelLoader(
        [[kiln_lifetimebound]] const fastgltf::Asset& model,
        std::size_t                                   scene_index,
        const glm::mat4x4& transform = glm::identity<glm::mat4x4>()
    );

    [[nodiscard]]
    auto geometry_buffer_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto material_buffer_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto transform_buffer_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto draw_command_count() const noexcept -> uint32_t;

    [[nodiscard]]
    auto geometry_buffer_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto material_buffer_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto transform_buffer_alignment() const noexcept -> uint32_t;

    auto set_geometry_buffer_byte_offset(uint32_t geometry_buffer_byte_offset) -> void;
    auto set_material_buffer_byte_offset(uint32_t material_buffer_byte_offset) -> void;
    auto set_transform_buffer_byte_offset(uint32_t transform_buffer_byte_offset) -> void;

    [[nodiscard]]
    auto geometry_writer() const noexcept [[kiln_lifetimebound]]
    -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto material_writer() const noexcept [[kiln_lifetimebound]]
    -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto transform_writer() const noexcept [[kiln_lifetimebound]]
    -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto draw_command_writer() const noexcept [[kiln_lifetimebound]]
    -> kiln::gfx::renderer::LazyCopy;

private:
    struct Offsets {
        std::optional<uint32_t> geometry_buffer_byte_offset;
        std::optional<uint32_t> material_buffer_byte_offset;
        std::optional<uint32_t> transform_buffer_byte_offset;
    };

    class Manifest {
    public:
        [[nodiscard]]
        static auto create(
            const fastgltf::Asset& model,
            std::size_t            scene_index,
            const glm::mat4x4&     transform
        ) -> Manifest;


        [[nodiscard]]
        auto geometry_buffer_size_bytes() const noexcept -> uint32_t;
        [[nodiscard]]
        auto material_buffer_size_bytes() const noexcept -> uint32_t;
        [[nodiscard]]
        auto transform_buffer_size_bytes() const noexcept -> uint32_t;
        [[nodiscard]]
        auto draw_command_count() const noexcept -> uint32_t;

        [[nodiscard]]
        auto geometry_buffer_alignment() const noexcept -> uint32_t;
        [[nodiscard]]
        auto material_buffer_alignment() const noexcept -> uint32_t;
        [[nodiscard]]
        auto transform_buffer_alignment() const noexcept -> uint32_t;

        auto write_geometry(const fastgltf::Asset& model, std::span<std::byte> out) const
            -> void;
        auto write_materials(const fastgltf::Asset& model, std::span<std::byte> out) const
            -> void;
        auto write_transforms(const fastgltf::Asset& model, std::span<std::byte> out) const
            -> void;
        auto write_draw_commands(
            const fastgltf::Asset& model,
            const Offsets&         global_offsets,
            std::span<std::byte>   out
        ) const -> void;

    private:
        using Writer = kiln::util::CopyableFunction<
            auto(
                const fastgltf::Asset&,
                std::span<const uint32_t, std::to_underlying(SupportedElementType::COUNT)>,
                std::span<std::byte>
            ) const->void>;
        using DrawCommandWriter = kiln::util::CopyableFunction<
            auto(const fastgltf::Asset&, const Offsets&, std::span<std::byte>) const->void>;

        static auto preprocess(
            const fastgltf::Asset& model,
            std::size_t            scene_index,
            Manifest&              manifest
        ) -> void;

        static auto process(
            const fastgltf::Asset& model,
            const glm::mat4x4&     transform,
            const fastgltf::Node&  node,
            Manifest&              manifest
        ) -> void;
        static auto process(
            const fastgltf::Asset& model,
            const glm::mat4x4&     transform,
            std::size_t            mesh_index,
            Manifest&              manifest
        ) -> void;
        static auto process(
            const fastgltf::Asset&     model,
            const fastgltf::Primitive& primitive,
            Manifest&                  manifest
        ) -> void;

        template <typename Element_T>
        [[nodiscard]]
        static auto element_writer_from(
            SupportedElementType element_type,
            std::size_t          accessor_index,
            uint32_t             element_byte_offset
        ) -> Writer;
        [[nodiscard]]
        static auto element_writer_from(
            SupportedElementType   element_type,
            fastgltf::AccessorType accessor_type,
            std::size_t            accessor_index,
            uint32_t               element_byte_offset
        ) -> Writer;
        [[nodiscard]]
        static auto color_vec3_writer_from(
            SupportedElementType element_type,
            std::size_t          accessor_index,
            uint32_t             element_byte_offset
        ) -> Writer;

        [[nodiscard]]
        auto shader_primitive_from(
            const Offsets&             global_offsets,
            const fastgltf::Primitive& primitive
        ) const -> shaders::Primitive;
        [[nodiscard]]
        auto vertex_offset_from(const fastgltf::Primitive& primitive) const -> uint32_t;


        uint32_t m_geometry_buffer_size_bytes{};
        uint32_t m_material_buffer_size_bytes{};
        uint32_t m_draw_command_count{};

        std::array<uint32_t, std::to_underlying(SupportedElementType::COUNT)>
                              m_element_buffer_byte_offsets{};
        std::vector<uint32_t> m_accessor_element_byte_offsets;
        std::vector<uint32_t> m_per_mesh_transform_offsets;
        std::vector<uint32_t> m_per_mesh_instance_counts;

        std::vector<Writer>      m_geometry_writers;
        std::vector<glm::mat4x4> m_transforms;
    };

    using DrawCommandWriterFactory =                                               //
        kiln::util::CopyableFunction<                                              //
            auto(const Offsets&) const noexcept -> kiln::gfx::renderer::LazyCopy   //
            >;


    std::reference_wrapper<const fastgltf::Asset> m_model;
    Manifest                                      m_manifest;
    Offsets                                       m_offsets;
};

}   // namespace demo
