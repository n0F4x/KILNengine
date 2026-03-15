module;

#include <functional>
#include <utility>

export module kiln.gfx.renderer.device.DevicePlugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.DeviceBuilder;
import kiln.gfx.vulkan.InstancePlugin;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::gfx::renderer {

export class DevicePlugin {
public:
    struct CreateInfo {
        bool headless{};
    };

    explicit DevicePlugin(bool headless = false);

    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self)
        -> util::forward_like_t<vulkan::DeviceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto operator->(this Self_T& self)
        -> util::const_like_t<vulkan::DeviceBuilder, Self_T>*;

    auto operator()(app::App& app) -> void;

private:
    bool                                           m_request_debug_messenger{};
    bool                                           m_headless{};
    vulkan::DeviceBuilder                          m_device_builder;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

template <typename Self_T>
auto DevicePlugin::operator*(this Self_T&& self)
    -> util::forward_like_t<vulkan::DeviceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.DevicePlugin::m_device_builder);
}

template <typename Self_T>
auto DevicePlugin::operator->(this Self_T& self)
    -> util::const_like_t<vulkan::DeviceBuilder, Self_T>*
{
    return &self.DevicePlugin::m_device_builder;
}

}   // namespace kiln::gfx::renderer
