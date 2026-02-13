export module kiln.gfx.renderer.Device;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export class Device {
public:
    Device(
        vk::raii::PhysicalDevice physical_device,
        vk::raii::Device         logical_device,
        vk::raii::Queue          graphics_queue
    );

private:
    vk::raii::PhysicalDevice m_physical_device;
    vk::raii::Device         m_logical_device;
    vk::raii::Queue          m_graphics_queue;
};

}   // namespace kiln::gfx::renderer
