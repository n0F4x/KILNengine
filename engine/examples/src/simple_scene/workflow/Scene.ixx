export module examples.simple_scene.workflow.Scene;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Buffer;

namespace demo {

export class Scene {
public:
    explicit Scene(
        const kiln::gfx::renderer::Device& device,
        kiln::gfx::renderer::Buffer&&      geometry_buffer,
        vk::DeviceSize                     index_byte_offset,
        vk::DeviceSize                     position_byte_offset,
        vk::DeviceSize                     vertex_byte_offset,
        kiln::gfx::renderer::Buffer&&      material_buffer,
        kiln::gfx::renderer::Buffer&&      primitive_buffer
    );


    [[nodiscard]]
    auto index_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto position_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto vertex_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto material_buffer_address() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto primitive_buffer_address() const noexcept -> vk::DeviceSize;


private:
    kiln::gfx::renderer::Buffer m_geometry_buffer;
    vk::DeviceSize              m_geometry_buffer_address;
    vk::DeviceSize              m_index_byte_offset;
    vk::DeviceSize              m_position_byte_offset;
    vk::DeviceSize              m_vertex_byte_offset;
    kiln::gfx::renderer::Buffer m_material_buffer;
    vk::DeviceSize              m_material_buffer_address;
    kiln::gfx::renderer::Buffer m_primitive_buffer;
    vk::DeviceSize              m_primitive_buffer_address;
};

}   // namespace demo
