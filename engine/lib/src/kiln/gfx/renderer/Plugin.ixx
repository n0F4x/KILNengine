module;

#include <functional>
#include <optional>
#include <print>
#include <utility>

export module kiln.gfx.renderer.Plugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.gfx.renderer.Device;
import kiln.gfx.renderer.PluginFailedError;
import kiln.gfx.vulkan.default_debug_messenger_callback;
import kiln.gfx.vulkan.Device;
import kiln.gfx.vulkan.DeviceBuilder;
import kiln.gfx.vulkan.InstancePlugin;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;
import kiln.util.Lazy;

namespace kiln::gfx::renderer {

export class Plugin {
public:
    explicit Plugin(vulkan::InstancePlugin& instance_plugin);

    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self)
        -> util::forward_like_t<vulkan::DeviceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto operator->(this Self_T& self)
        -> util::const_like_t<vulkan::DeviceBuilder, Self_T>*;

    auto request_debug_messenger() -> bool;

    auto operator()(app::App& app) const -> void;

private:
    std::reference_wrapper<vulkan::InstancePlugin> m_instance_plugin_ref;
    bool                                           m_request_debug_messenger{};
    vulkan::DeviceBuilder                          m_device_builder;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

Plugin::Plugin(vulkan::InstancePlugin& instance_plugin)
    : m_instance_plugin_ref{ instance_plugin }
{
#ifdef KILN_DEBUG
    request_debug_messenger();
#endif

    m_device_builder.request_graphics_queue();
}

template <typename Self_T>
auto Plugin::operator*(this Self_T&& self)
    -> util::forward_like_t<vulkan::DeviceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.Plugin::m_device_builder);
}

template <typename Self_T>
auto Plugin::operator->(this Self_T& self)
    -> util::const_like_t<vulkan::DeviceBuilder, Self_T>*
{
    return &self.Plugin::m_device_builder;
}

auto Plugin::request_debug_messenger() -> bool
{
    if (m_request_debug_messenger)
    {
        return true;
    }

    if (!m_instance_plugin_ref.get()->enable_extension(vk::EXTDebugUtilsExtensionName))
    {
        return false;
    }

    m_request_debug_messenger = true;

    return true;
}

auto Plugin::operator()(app::App& app) const -> void
{
    const vk::raii::Instance& instance{ app.resources().at<vk::raii::Instance>() };

    constexpr static vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = vulkan::default_debug_messenger_callback,
    };

    vk::raii::DebugUtilsMessengerEXT debug_messenger{
        m_request_debug_messenger
            ? vulkan::check_result(
                  instance.createDebugUtilsMessengerEXT(debug_messenger_create_info)
              )
            : vk::raii::DebugUtilsMessengerEXT{ nullptr }
    };

    auto [physical_device, logical_device, queues, enabled_capabilities] =
        m_device_builder.build(instance).value_or(
            util::Lazy{
                [] [[noreturn]] -> vulkan::Device
                {
                    throw PluginFailedError{ "No supported device found" };   //
                }   //
            }
        );

    app.resources().emplace<Device>(
        std::move(debug_messenger),
        std::move(physical_device),
        std::move(logical_device),
        std::move(queues),
        std::move(enabled_capabilities)
    );
}

}   // namespace kiln::gfx::renderer
