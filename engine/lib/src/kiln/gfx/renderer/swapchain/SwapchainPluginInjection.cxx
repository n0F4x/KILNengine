module;

#include <span>

module kiln.gfx.renderer.swapchain.SwapchainPluginInjection;

import kiln.gfx.renderer.swapchain.SwapchainPluginFailedError;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.util.Lazy;
import kiln.util.StringLiteral;
import kiln.wsi.Context;
import kiln.wsi.vulkan_instance_extensions;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

auto SwapchainPluginInjection::operator()(
    vulkan::InstancePlugin& instance_plugin,
    const wsi::Plugin&      wsi_plugin,
    DevicePlugin&           device_plugin
) -> SwapchainPlugin
{
    for (const char* extension_name :
         wsi::vulkan_instance_extensions(wsi_plugin.context())
             .value_or(
                 util::Lazy{
                     [] -> std::span<const char* const>
                     {
                         throw SwapchainPluginFailedError{
                             "Vulkan surface creation is not supported"
                         };
                     }   //
                 }
             ))
    {
        {
            instance_plugin->enable_extension(
                util::StringLiteral::unsafe_create(extension_name)
            );
        }
    }

    device_plugin->enable_extension(vk::KHRSwapchainExtensionName);

    return SwapchainPlugin{};
}

}   // namespace kiln::gfx::renderer
