module;

#include <string>

export module kiln.gfx.renderer.device.Device;

import vulkan_hpp;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;

namespace kiln::gfx::renderer {

export class Device {
public:
    Device(
        vk::raii::PhysicalDevice           physical_device,
        vk::raii::Device                   logical_device,
        vulkan::PhysicalDeviceCapabilities capabilities
    );

    [[nodiscard]]
    auto name() const -> std::string;
    [[nodiscard]]
    auto physical_device() const noexcept -> const vk::raii::PhysicalDevice&;
    [[nodiscard]]
    auto logical_device() const noexcept -> const vk::raii::Device&;
    [[nodiscard]]
    auto capabilities() const noexcept -> const vulkan::PhysicalDeviceCapabilities&;

private:
    vk::raii::PhysicalDevice           m_physical_device;
    vk::raii::Device                   m_logical_device;
    vulkan::PhysicalDeviceCapabilities m_capabilities;
};

}   // namespace kiln::gfx::renderer
