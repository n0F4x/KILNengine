module;

#include <utility>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.device.DevicePlugin;

import kiln.gfx.renderer.device.DevicePluginFailedError;
import kiln.gfx.vulkan.default_debug_messenger_callback;
import kiln.gfx.vulkan.Device;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;
import kiln.util.Lazy;
import kiln.wsi.Context;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

DevicePlugin::DevicePlugin(const bool headless) : m_headless{ headless } {}

auto DevicePlugin::build(
    const vk::raii::Instance&             instance,
    const util::OptionalRef<wsi::Context> wsi_context
) -> Device
{
    if (!m_headless)
    {
        PRECOND(wsi_context.has_value());

        m_device_builder.ensure_queue(
            [wsi_context, &instance](
                const vk::raii::PhysicalDevice& physical_device,
                const vulkan::QueueFamilyIndex  queue_family_index,
                const vk::QueueFamilyProperties2&
            ) -> bool
            {
                return wsi::vulkan_queue_family_supports_presenting(
                    *wsi_context, instance, physical_device, queue_family_index
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

    return Device{
        std::move(physical_device),
        std::move(logical_device),
        std::move(queues),
        std::move(enabled_capabilities),
    };
}

}   // namespace kiln::gfx::renderer
