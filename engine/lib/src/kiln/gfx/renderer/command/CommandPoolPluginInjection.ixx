module;

#include <cstdint>

export module kiln.gfx.renderer.command.CommandPoolPluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandPoolPlugin;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.vulkan.InstancePlugin;

namespace kiln::gfx::renderer {

export class CommandPoolPluginInjection {
public:
    explicit CommandPoolPluginInjection(const uint8_t number_of_frames)
        : m_number_of_frames{ number_of_frames }
    {
    }

    [[nodiscard]]
    auto operator()(
        vulkan::InstancePlugin& instance_plugin,
        DevicePlugin&           device_plugin
    ) const -> CommandPoolPlugin
    {
        instance_plugin->request_api_version(vk::ApiVersion13);
        device_plugin->require_minimum_version(vk::ApiVersion13);
        device_plugin->request_graphics_queue();
        device_plugin->request_host_to_device_transfer_queue();

        return CommandPoolPlugin{ m_number_of_frames };
    }

private:
    uint8_t m_number_of_frames;
};

}   // namespace kiln::gfx::renderer
