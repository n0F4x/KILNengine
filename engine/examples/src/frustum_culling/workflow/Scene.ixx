module;

#include <cstdint>

export module examples.frustum_culling.workflow.Scene;

import vulkan;

import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.presentation.RenderSurface;

namespace demo {

export class Scene {
public:
    explicit Scene() = default;
    explicit Scene(
        const kiln::gfx::renderer::Device& device,
        uint32_t                           max_instance_count,
        kiln::gfx::renderer::Buffer&&      instance_draw_command_indices_buffer,
        kiln::gfx::renderer::Buffer&&      instance_sphere_bounding_volumes_buffer,
        kiln::gfx::renderer::Buffer&&      draw_command_instance_counts_buffer,
        kiln::gfx::renderer::Buffer&&      instance_draw_command_offsets_buffer,
        uint32_t                           max_draw_command_count,
        kiln::gfx::renderer::Buffer&&      original_draw_commands_buffer,
        kiln::gfx::renderer::Buffer&&      instance_counter_buffer,
        kiln::gfx::renderer::Buffer&&      generated_draw_commands_buffer,
        kiln::gfx::renderer::Buffer&&      draw_command_instance_offsets_buffer,
        kiln::gfx::renderer::Buffer&&      instance_indices_buffer,
        kiln::gfx::renderer::Buffer&&      geometry_buffer,
        kiln::gfx::renderer::Buffer&&      materials_buffer,
        kiln::gfx::renderer::Buffer&&      instance_buffer
    );


    [[nodiscard]]
    auto draw_command_instance_counts_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;
    [[nodiscard]]
    auto instance_draw_command_offsets_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;
    [[nodiscard]]
    auto max_instance_count() const noexcept -> uint32_t;
    [[nodiscard]]
    auto instance_draw_command_indices_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto instance_sphere_bounding_volumes_buffer_address() const noexcept
        -> vk::DeviceSize;
    [[nodiscard]]
    auto draw_command_instance_counts_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto instance_draw_command_offsets_buffer_address() const noexcept -> vk::DeviceSize;

    [[nodiscard]]
    auto instance_counter_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;
    [[nodiscard]]
    auto draw_command_count_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;
    [[nodiscard]]
    auto max_draw_command_count() const noexcept -> uint32_t;
    [[nodiscard]]
    auto original_draw_commands_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto instance_counter_buffer_address() const noexcept -> vk::DeviceAddress;
    [[nodiscard]]
    auto draw_command_count_buffer_address() const noexcept -> vk::DeviceAddress;
    [[nodiscard]]
    auto generated_draw_commands_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto draw_command_instance_offsets_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto generated_draw_commands_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;
    [[nodiscard]]
    auto draw_command_instance_offsets_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;

    [[nodiscard]]
    auto instance_indices_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto instance_indices_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;

    [[nodiscard]]
    auto geometry_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto materials_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto instance_buffer_address() const noexcept -> vk::DeviceSize;

private:
    uint32_t                          m_max_instance_count;
    kiln::gfx::renderer::Buffer       m_instance_draw_command_indices_buffer;
    vk::DeviceSize                    m_instance_draw_command_indices_buffer_address{};
    kiln::gfx::renderer::Buffer       m_instance_sphere_bounding_volumes_buffer;
    vk::DeviceSize                    m_instance_sphere_bounding_volumes_buffer_address{};
    kiln::gfx::renderer::Buffer       m_draw_command_instance_counts_buffer;
    kiln::gfx::renderer::BufferRegion m_draw_command_instance_counts_buffer_region{
        m_draw_command_instance_counts_buffer
    };
    vk::DeviceSize                    m_draw_command_instance_counts_buffer_address{};
    kiln::gfx::renderer::Buffer       m_instance_draw_command_offsets_buffer;
    kiln::gfx::renderer::BufferRegion m_instance_draw_command_offsets_buffer_region{
        m_instance_draw_command_offsets_buffer
    };
    vk::DeviceSize m_instance_draw_command_offsets_buffer_address{};

    uint32_t                          m_max_draw_command_count{};
    kiln::gfx::renderer::Buffer       m_original_draw_commands_buffer;
    vk::DeviceSize                    m_original_draw_commands_buffer_address{};
    kiln::gfx::renderer::Buffer       m_instance_counter_buffer;
    kiln::gfx::renderer::BufferRegion m_instance_counter_buffer_region{
        m_instance_counter_buffer
    };
    vk::DeviceSize                    m_instance_counter_buffer_address{};
    kiln::gfx::renderer::Buffer       m_generated_draw_commands_buffer;
    kiln::gfx::renderer::BufferRegion m_draw_command_count_buffer_region{
        m_generated_draw_commands_buffer
    };
    vk::DeviceSize                    m_draw_command_count_buffer_address{};
    vk::DeviceSize                    m_generated_draw_commands_buffer_address{};
    kiln::gfx::renderer::BufferRegion m_generated_draw_commands_buffer_region{
        m_generated_draw_commands_buffer
    };
    kiln::gfx::renderer::Buffer       m_draw_command_instance_offsets_buffer;
    kiln::gfx::renderer::BufferRegion m_draw_command_instance_offsets_buffer_region{
        m_draw_command_instance_offsets_buffer
    };
    vk::DeviceSize m_draw_command_instance_offsets_buffer_address{};

    kiln::gfx::renderer::Buffer       m_instance_indices_buffer;
    kiln::gfx::renderer::BufferRegion m_instance_indices_buffer_region{
        m_instance_indices_buffer
    };
    vk::DeviceSize m_instance_indices_buffer_address{};

    kiln::gfx::renderer::Buffer m_geometry_buffer;
    vk::DeviceSize              m_geometry_buffer_address{};
    kiln::gfx::renderer::Buffer m_materials_buffer;
    vk::DeviceSize              m_materials_buffer_address{};
    kiln::gfx::renderer::Buffer m_instance_buffer;
    vk::DeviceSize              m_instance_buffer_address{};
};

}   // namespace demo
