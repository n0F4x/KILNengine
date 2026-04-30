module;

#include <functional>
#include <optional>

#include <fastgltf/core.hpp>

export module examples.simple_scene.workflow.GltfModelLoader;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.memory.LazyCopy;
import kiln.util.containers.CopyableFunction;

import examples.simple_scene.shaders;

namespace demo {

export class GltfModelLoader {
public:
    explicit GltfModelLoader(const fastgltf::Asset& model);

    [[nodiscard]]
    auto indices_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto positions_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto vertices_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto materials_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto transforms_size_bytes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto draw_command_count() const noexcept -> uint32_t;

    [[nodiscard]]
    auto index_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto position_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto vertex_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto material_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto transform_alignment() const noexcept -> uint32_t;

    auto set_index_offset(uint32_t index_offset) -> void;
    auto set_position_offset(uint32_t position_offset) -> void;
    auto set_vertex_offset(uint32_t vertex_offset) -> void;
    auto set_material_offset(uint32_t material_offset) -> void;
    auto set_transform_offset(uint32_t transform_offset) -> void;

    [[nodiscard]]
    auto index_writer() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto position_writer() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto vertex_writer() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto material_writer() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto transform_writer() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto draw_command_writer() const noexcept -> kiln::gfx::renderer::LazyCopy;

private:
    using Writer = kiln::util::CopyableFunction<auto(std::span<std::byte>) const->void>;

    struct Offsets {
        std::optional<uint32_t> index_offset;
        std::optional<uint32_t> position_offset;
        std::optional<uint32_t> vertex_offset;
        std::optional<uint32_t> material_offset;
        std::optional<uint32_t> transform_offset;
    };

    using DrawCommandWriterFactory =                                               //
        kiln::util::CopyableFunction<                                              //
            auto(const Offsets&) const noexcept -> kiln::gfx::renderer::LazyCopy   //
            >;


    uint32_t m_indices_size_bytes;
    uint32_t m_positions_size_bytes;
    uint32_t m_vertices_size_bytes;
    uint32_t m_materials_size_bytes;
    uint32_t m_transforms_size_bytes;
    uint32_t m_draw_command_count;

    Writer                   m_write_indices;
    Writer                   m_write_positions;
    Writer                   m_write_vertices;
    Writer                   m_write_materials;
    Writer                   m_write_transforms;
    DrawCommandWriterFactory m_make_draw_command_writer;

    Offsets m_offsets;


    [[nodiscard]]
    static auto make_index_writer(const fastgltf::Asset&) -> Writer;
    [[nodiscard]]
    static auto make_position_writer(const fastgltf::Asset&) -> Writer;
    [[nodiscard]]
    static auto make_vertex_writer(const fastgltf::Asset&) -> Writer;
    [[nodiscard]]
    static auto make_material_writer(const fastgltf::Asset&) -> Writer;
    [[nodiscard]]
    static auto make_transform_writer(const fastgltf::Asset&) -> Writer;
    [[nodiscard]]
    static auto make_draw_command_writer_factory(const fastgltf::Asset&)
        -> DrawCommandWriterFactory;
};

}   // namespace demo
