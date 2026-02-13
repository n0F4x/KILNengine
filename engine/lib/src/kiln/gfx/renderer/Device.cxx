module;

#include <utility>

module kiln.gfx.renderer.Device;

namespace kiln::gfx::renderer {

Device::Device(
    vk::raii::PhysicalDevice physical_device,
    vk::raii::Device         logical_device,
    vk::raii::Queue          graphics_queue
)
    : m_physical_device{ std::move(physical_device) },
      m_logical_device{ std::move(logical_device) },
      m_graphics_queue{ std::move(graphics_queue) }
{
}

}   // namespace kiln::gfx::renderer
