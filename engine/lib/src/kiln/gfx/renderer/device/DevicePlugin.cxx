module;

#include <utility>

module kiln.gfx.renderer.device.DevicePlugin;

import kiln.gfx.renderer.device.DevicePluginFailedError;
import kiln.gfx.vulkan.default_debug_messenger_callback;
import kiln.gfx.vulkan.Device;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.Lazy;
import kiln.wsi.Context;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

DevicePlugin::DevicePlugin(vulkan::InstancePlugin& instance_plugin, bool headless)
    : m_instance_plugin_ref{ instance_plugin },
      m_headless{ headless }
{
#ifdef KILN_DEBUG
    request_debug_messenger();
#endif
}

auto DevicePlugin::request_debug_messenger() -> bool
{
    if (m_request_debug_messenger)
    {
        return true;
    }

    if (!m_instance_plugin_ref.get()->enable_extension_if_available(
            vk::EXTDebugUtilsExtensionName
        ))
    {
        return false;
    }

    m_request_debug_messenger = true;

    return true;
}

auto DevicePlugin::operator()(app::App& app) -> void
{
    const vk::raii::Instance& instance{ app.context().at<vk::raii::Instance>() };

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
        wsi::Context& wsi_context{ app.context().at<wsi::Context>() };

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
                    throw DevicePluginFailedError{ "No supported device found" };   //
                }   //
            }
        );

    app.context().emplace<Device>(
        std::move(debug_messenger),
        std::move(physical_device),
        std::move(logical_device),
        std::move(queues),
        std::move(enabled_capabilities)
    );
}

}   // namespace kiln::gfx::renderer
