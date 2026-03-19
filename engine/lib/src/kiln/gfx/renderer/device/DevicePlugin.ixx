module;

#include <memory_resource>
#include <utility>

export module kiln.gfx.renderer.device.DevicePlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.DeviceBuilder;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::gfx::renderer {

export class DevicePlugin : public app::PluginInterface {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;


    DevicePlugin() = default;
    explicit DevicePlugin(const allocator_type& allocator);
    DevicePlugin(const DevicePlugin&, const allocator_type& allocator);
    DevicePlugin(DevicePlugin&&, const allocator_type& allocator);


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self)
        -> util::forward_like_t<vulkan::DeviceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto operator->(this Self_T& self)
        -> util::const_like_t<vulkan::DeviceBuilder, Self_T>*;

    [[nodiscard]]
    auto operator()(const vk::raii::Instance& instance) const -> Device;

private:
    vulkan::DeviceBuilder m_device_builder;
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
