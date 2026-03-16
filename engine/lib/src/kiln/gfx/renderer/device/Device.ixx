module;

#include <string>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.device.Device;

import vulkan_hpp;

import kiln.gfx.renderer.device.GraphicsQueueRef;
import kiln.gfx.renderer.device.TransferQueueRef;
import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueGroup;

namespace kiln::gfx::renderer {

export class Device {
public:
    Device(
        vk::raii::PhysicalDevice           physical_device,
        vk::raii::Device                   logical_device,
        vulkan::QueueGroup                 queues,
        vulkan::PhysicalDeviceCapabilities capabilities
    );

    [[nodiscard]]
    auto name() const -> std::string;
    [[nodiscard]]
    auto graphics_queue() const [[kiln_lifetimebound]] -> GraphicsQueueRef;
    [[nodiscard]]
    auto host_to_device_transfer_queue() const [[kiln_lifetimebound]] -> TransferQueueRef;
    [[nodiscard]]
    auto physical_device() const noexcept -> const vk::raii::PhysicalDevice&;
    [[nodiscard]]
    auto logical_device() const noexcept -> const vk::raii::Device&;
    [[nodiscard]]
    auto capabilities() const noexcept -> const vulkan::PhysicalDeviceCapabilities&;

private:
    vk::raii::PhysicalDevice           m_physical_device;
    vk::raii::Device                   m_logical_device;
    vulkan::QueueGroup                 m_queues;
    vulkan::PhysicalDeviceCapabilities m_capabilities;
};

}   // namespace kiln::gfx::renderer
