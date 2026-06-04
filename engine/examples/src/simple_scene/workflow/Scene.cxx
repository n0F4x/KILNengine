module;

#include <bit>
#include <utility>

#include <kiln/util/contract_macros.hpp>

module examples.simple_scene.workflow.Scene;

import vulkan_hpp;

import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;

import examples.simple_scene.shaders;

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

template <typename T>
[[nodiscard]]
constexpr T align_up(const T value, const T alignment)
{
    PRECOND(std::has_single_bit(alignment));
    return value + (alignment - 1) & ~(alignment - 1);
}

Scene::Scene(
    const kiln::gfx::renderer::Device& device,
    const uint32_t                     max_instance_count,
    kiln::gfx::renderer::Buffer&&      instance_draw_command_indices_buffer,
    kiln::gfx::renderer::Buffer&&      instance_sphere_bounding_volumes_buffer,
    kiln::gfx::renderer::Buffer&&      draw_command_instance_counts_buffer,
    kiln::gfx::renderer::Buffer&&      instance_draw_command_offsets_buffer,
    const uint32_t                     max_draw_command_count,
    kiln::gfx::renderer::Buffer&&      original_draw_commands_buffer,
    kiln::gfx::renderer::Buffer&&      instance_counter_buffer,
    kiln::gfx::renderer::Buffer&&      generated_draw_commands_buffer,
    kiln::gfx::renderer::Buffer&&      draw_command_instance_offsets_buffer,
    kiln::gfx::renderer::Buffer&&      instance_indices_buffer,
    kiln::gfx::renderer::Buffer&&      geometry_buffer,
    kiln::gfx::renderer::Buffer&&      materials_buffer,
    kiln::gfx::renderer::Buffer&&      instance_buffer
)
    : m_max_instance_count{ max_instance_count },
      m_instance_draw_command_indices_buffer{
          std::move(instance_draw_command_indices_buffer)
      },
      m_instance_draw_command_indices_buffer_address{
          address_of_buffer(device, m_instance_draw_command_indices_buffer)
      },
      m_instance_sphere_bounding_volumes_buffer{
          std::move(instance_sphere_bounding_volumes_buffer)
      },
      m_instance_sphere_bounding_volumes_buffer_address{
          address_of_buffer(device, m_instance_sphere_bounding_volumes_buffer)
      },
      m_draw_command_instance_counts_buffer{
          std::move(draw_command_instance_counts_buffer)
      },
      m_draw_command_instance_counts_buffer_address{
          address_of_buffer(device, m_draw_command_instance_counts_buffer)
      },
      m_instance_draw_command_offsets_buffer{
          std::move(instance_draw_command_offsets_buffer)
      },
      m_instance_draw_command_offsets_buffer_address{
          address_of_buffer(device, m_instance_draw_command_offsets_buffer)
      },
      m_max_draw_command_count{ max_draw_command_count },
      m_original_draw_commands_buffer{ std::move(original_draw_commands_buffer) },
      m_original_draw_commands_buffer_address{
          address_of_buffer(device, m_original_draw_commands_buffer)
      },
      m_instance_counter_buffer{ std::move(instance_counter_buffer) },
      m_instance_counter_buffer_address{
          address_of_buffer(device, m_instance_counter_buffer)
      },
      m_generated_draw_commands_buffer{ std::move(generated_draw_commands_buffer) },
      m_draw_command_count_buffer_region{
          m_generated_draw_commands_buffer,
          0,
          sizeof(uint32_t),
      },
      m_draw_command_count_buffer_address{
          address_of_buffer(device, m_generated_draw_commands_buffer)
      },
      m_generated_draw_commands_buffer_address{
          align_up(
              address_of_buffer(device, m_generated_draw_commands_buffer)
                  + sizeof(uint32_t),
              alignof(shaders::DrawCommand)
          ),
      },
      m_generated_draw_commands_buffer_region{
          m_generated_draw_commands_buffer,
          align_up(sizeof(uint32_t), alignof(shaders::DrawCommand))
      },
      m_draw_command_instance_offsets_buffer{
          std::move(draw_command_instance_offsets_buffer)
      },
      m_draw_command_instance_offsets_buffer_address{
          address_of_buffer(device, m_draw_command_instance_offsets_buffer)
      },
      m_instance_indices_buffer{ std::move(instance_indices_buffer) },
      m_instance_indices_buffer_address{
          address_of_buffer(device, m_instance_indices_buffer)
      },
      m_geometry_buffer{ std::move(geometry_buffer) },
      m_geometry_buffer_address{ address_of_buffer(device, m_geometry_buffer) },
      m_materials_buffer{ std::move(materials_buffer) },
      m_materials_buffer_address{ address_of_buffer(device, m_materials_buffer) },
      m_instance_buffer{ std::move(instance_buffer) },
      m_instance_buffer_address{ address_of_buffer(device, m_instance_buffer) }
{
}

auto Scene::draw_command_instance_counts_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_draw_command_instance_counts_buffer_region;
}

auto Scene::instance_draw_command_offsets_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_instance_draw_command_offsets_buffer_region;
}

auto Scene::max_instance_count() const noexcept -> uint32_t
{
    return m_max_instance_count;
}

auto Scene::instance_draw_command_indices_buffer_address() const noexcept
    -> vk::DeviceSize
{
    return m_instance_draw_command_indices_buffer_address;
}

auto Scene::instance_sphere_bounding_volumes_buffer_address() const noexcept
    -> vk::DeviceSize
{
    return m_instance_sphere_bounding_volumes_buffer_address;
}

auto Scene::draw_command_instance_counts_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_draw_command_instance_counts_buffer_address;
}

auto Scene::instance_draw_command_offsets_buffer_address() const noexcept
    -> vk::DeviceSize
{
    return m_instance_draw_command_offsets_buffer_address;
}

auto Scene::instance_counter_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_instance_counter_buffer_region;
}

auto Scene::draw_command_count_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_draw_command_count_buffer_region;
}

auto Scene::max_draw_command_count() const noexcept -> uint32_t
{
    return m_max_draw_command_count;
}

auto Scene::original_draw_commands_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_original_draw_commands_buffer_address;
}

auto Scene::instance_counter_buffer_address() const noexcept -> vk::DeviceAddress
{
    return m_instance_counter_buffer_address;
}

auto Scene::draw_command_count_buffer_address() const noexcept -> vk::DeviceAddress
{
    return m_draw_command_count_buffer_address;
}

auto Scene::generated_draw_commands_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_generated_draw_commands_buffer_address;
}

auto Scene::draw_command_instance_offsets_buffer_address() const noexcept
    -> vk::DeviceSize
{
    return m_draw_command_instance_offsets_buffer_address;
}

auto Scene::generated_draw_commands_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_generated_draw_commands_buffer_region;
}

auto Scene::draw_command_instance_offsets_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_draw_command_instance_offsets_buffer_region;
}

auto Scene::instance_indices_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_instance_indices_buffer_address;
}

auto Scene::instance_indices_buffer_region() const noexcept
    -> const kiln::gfx::renderer::BufferRegion&
{
    return m_instance_indices_buffer_region;
}

auto Scene::geometry_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_geometry_buffer_address;
}

auto Scene::materials_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_materials_buffer_address;
}

auto Scene::instance_buffer_address() const noexcept -> vk::DeviceSize
{
    return m_instance_buffer_address;
}

}   // namespace demo
