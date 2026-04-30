module;

#include <functional>
#include <optional>

#include <fastgltf/core.hpp>

export module examples.simple_scene.workflow.GltfModelLoader;

import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.memory.LazyCopy;

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
    auto draw_command_size_bytes() const noexcept -> uint32_t;

    [[nodiscard]]
    auto index_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto position_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto vertex_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto material_alignment() const noexcept -> uint32_t;
    [[nodiscard]]
    auto draw_command_alignment() const noexcept -> uint32_t;

    auto set_index_offset(uint32_t index_offset) -> void;
    auto set_position_offset(uint32_t position_offset) -> void;
    auto set_vertex_offset(uint32_t vertex_offset) -> void;
    auto set_material_offset(uint32_t material_offset) -> void;

    [[nodiscard]]
    auto index_offset() const -> std::optional<uint32_t>;
    [[nodiscard]]
    auto position_offset() const -> std::optional<uint32_t>;
    [[nodiscard]]
    auto vertex_offset() const -> std::optional<uint32_t>;
    [[nodiscard]]
    auto material_offset() const -> std::optional<uint32_t>;

    [[nodiscard]]
    auto lazy_indices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto lazy_positions_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto lazy_vertices_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto lazy_materials_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;
    [[nodiscard]]
    auto lazy_draw_commands_copy() const noexcept -> kiln::gfx::renderer::LazyCopy;

    [[nodiscard]]
    auto max_draw_count() const noexcept -> uint32_t;

private:
    std::reference_wrapper<const fastgltf::Asset> m_model;
    std::optional<uint32_t>                       m_index_offset;
    std::optional<uint32_t>                       m_position_offset;
    std::optional<uint32_t>                       m_vertex_offset;
    std::optional<uint32_t>                       m_material_offset;


    [[nodiscard]]
    auto draw_command() const noexcept -> shaders::DrawCommand;
};

}   // namespace demo
