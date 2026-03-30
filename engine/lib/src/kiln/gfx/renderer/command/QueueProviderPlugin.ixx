module;

#include <cstdint>
#include <functional>
#include <optional>

export module kiln.gfx.renderer.command.QueueProviderPlugin;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.command.CommandPlugin;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

struct QueueInfo {
    bool                                                              requested{};
    std::optional<std::pair<vulkan::QueueFamilyIndex, std::uint32_t>> callback_result;
};

export class QueueProviderPlugin : public app::PluginInterface {
public:
    [[nodiscard]]
    static auto
        create_plugin(DevicePlugin& device_plugin, const CommandPlugin& command_plugin)
            -> QueueProviderPlugin;

    explicit QueueProviderPlugin(DevicePlugin& device_plugin);

    auto require_graphics_queue() -> void;

    [[nodiscard]]
    auto operator()(const Device& device) const -> QueueProvider;

private:
    std::reference_wrapper<DevicePlugin> m_device_plugin_ref;

    QueueInfo m_graphics_queue_info;
};

}   // namespace kiln::gfx::renderer
