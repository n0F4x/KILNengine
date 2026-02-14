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
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;
import kiln.util.Lazy;
import kiln.util.OptionalRef;
import kiln.wsi.Context;
import kiln.wsi.vulkan_queue_family_supports_presenting;
import kiln.wsi.Plugin;

namespace kiln::gfx::renderer {

export class Plugin {
public:
    struct CreateInfo {
        bool headless{};
    };

    explicit Plugin(vulkan::InstancePlugin& instance_plugin, bool headless = false);

    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self)
        -> util::forward_like_t<vulkan::DeviceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto operator->(this Self_T& self)
        -> util::const_like_t<vulkan::DeviceBuilder, Self_T>*;

    auto request_debug_messenger() -> bool;

    auto operator()(app::App& app) -> void;

private:
    std::reference_wrapper<vulkan::InstancePlugin> m_instance_plugin_ref;
    bool                                           m_request_debug_messenger{};
    bool                                           m_headless{};
    vulkan::DeviceBuilder                          m_device_builder;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

Plugin::Plugin(vulkan::InstancePlugin& instance_plugin, bool headless)
    : m_instance_plugin_ref{ instance_plugin },
      m_headless{ headless }
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

auto Plugin::operator()(app::App& app) -> void
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

    if (!m_headless)
    {
        wsi::Context& wsi_context{ app.resources().at<wsi::Context>() };

        m_device_builder.enable_extension(vk::KHRSwapchainExtensionName);
        m_device_builder.ensure_queue(
            [&wsi_context, &instance](
                const vk::raii::PhysicalDevice& physical_device,
                const vulkan::QueueFamilyIndex  queue_family_index,
                const vk::QueueFamilyProperties2&
            ) -> bool
            {
                return wsi::vulkan_queue_family_supports_presenting(
                    wsi_context, instance, physical_device, queue_family_index
                );
            }
        );
    }

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
