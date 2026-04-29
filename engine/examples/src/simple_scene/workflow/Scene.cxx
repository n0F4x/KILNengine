module;

#include <utility>

module examples.simple_scene.workflow.Scene;

import vulkan_hpp;

import kiln.gfx.vulkan.result.check_result;

namespace demo {

[[nodiscard]]
auto address_of_buffer(
    const kiln::gfx::renderer::Device& device,
    kiln::gfx::renderer::Buffer&       buffer
) -> vk::DeviceSize
{
    return buffer.get() == nullptr
             ? vk::DeviceSize{}
             : device.logical_device().getBufferAddress(
                   vk::BufferDeviceAddressInfo{ .buffer = buffer.get() }
               );
}

Scene::Scene(
    const kiln::gfx::renderer::Device& device,
    kiln::gfx::renderer::Buffer&&      geometry_buffer,
    const vk::DeviceSize               index_byte_offset,
    const vk::DeviceSize               position_byte_offset,
    const vk::DeviceSize               vertex_byte_offset,
    kiln::gfx::renderer::Buffer&&      material_buffer,
    kiln::gfx::renderer::Buffer&&      transform_buffer,
    kiln::gfx::renderer::Buffer&&      draw_command_buffer,
    const uint32_t                     draw_count
)
    : m_geometry_buffer{ std::move(geometry_buffer) },
      m_geometry_buffer_address{ address_of_buffer(device, m_geometry_buffer) },
      m_index_byte_offset{ index_byte_offset },
      m_position_byte_offset{ position_byte_offset },
      m_vertex_byte_offset{ vertex_byte_offset },
      m_material_buffer{ std::move(material_buffer) },
      m_material_buffer_address{ address_of_buffer(device, m_material_buffer) },
      m_transform_buffer{ std::move(transform_buffer) },
      m_transform_buffer_address{ address_of_buffer(device, m_transform_buffer) },
      m_draw_command_buffer{ std::move(draw_command_buffer) },
      m_draw_command_buffer_address{ address_of_buffer(device, m_draw_command_buffer) },
      m_draw_command_buffer_region{ m_draw_command_buffer },
      m_draw_count{ draw_count }
{
}

auto Scene::index_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_geometry_buffer_address == 0
             ? m_geometry_buffer_address
             : m_geometry_buffer_address + m_index_byte_offset;
}

auto Scene::position_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_geometry_buffer_address == 0
             ? m_geometry_buffer_address
             : m_geometry_buffer_address + m_position_byte_offset;
}

auto Scene::vertex_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_geometry_buffer_address == 0
             ? m_geometry_buffer_address
             : m_geometry_buffer_address + m_vertex_byte_offset;
}

auto Scene::material_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_material_buffer_address;
}

auto Scene::transform_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_transform_buffer_address;
}

auto Scene::draw_command_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_draw_command_buffer_address;
}

auto Scene::draw_command_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_draw_command_buffer_region;
}

auto Scene::draw_count() const noexcept -> uint32_t
{
    return m_draw_count;
}

}   // namespace demo
