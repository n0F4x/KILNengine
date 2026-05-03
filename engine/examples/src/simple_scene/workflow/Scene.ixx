module;

#include <cstdint>

export module examples.simple_scene.workflow.Scene;

import vulkan_hpp;

import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Buffer;
import kiln.gfx.renderer.memory.BufferRegion;
import kiln.gfx.renderer.presentation.RenderSurface;

namespace demo {

export class Scene {
public:
    explicit Scene(
        const kiln::gfx::renderer::Device& device,
        kiln::gfx::renderer::Buffer&&      geometry_buffer,
        kiln::gfx::renderer::Buffer&&      material_buffer,
        kiln::gfx::renderer::Buffer&&      instance_buffer,
        kiln::gfx::renderer::Buffer&&      draw_command_buffer,
        uint32_t                           draw_command_count_size,
        uint32_t                           max_draw_count
    );


    [[nodiscard]]
    auto geometry_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto material_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto instance_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto draw_command_buffer_address() const noexcept -> vk::DeviceSize;

    [[nodiscard]]
    auto draw_command_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;
    [[nodiscard]]
    auto draw_command_count_buffer_region() const noexcept
        -> const kiln::gfx::renderer::BufferRegion&;
    [[nodiscard]]
    auto max_draw_count() const noexcept -> uint32_t;

private:
    kiln::gfx::renderer::Buffer       m_geometry_buffer;
    vk::DeviceSize                    m_geometry_buffer_address;
    kiln::gfx::renderer::Buffer       m_material_buffer;
    vk::DeviceSize                    m_material_buffer_address;
    kiln::gfx::renderer::Buffer       m_instance_buffer;
    vk::DeviceSize                    m_instance_buffer_address;
    kiln::gfx::renderer::Buffer       m_draw_command_buffer;
    vk::DeviceSize                    m_draw_command_buffer_address;
    kiln::gfx::renderer::BufferRegion m_draw_command_buffer_region;
    kiln::gfx::renderer::BufferRegion m_draw_command_count_buffer_region;
    uint32_t                          m_max_draw_count;
};

}   // namespace demo
