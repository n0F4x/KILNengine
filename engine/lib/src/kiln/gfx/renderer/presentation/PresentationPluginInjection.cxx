module;

#include <span>

module kiln.gfx.renderer.presentation.PresentationPluginInjection;

import vulkan_hpp;

import kiln.gfx.renderer.presentation.PresentationPluginFailedError;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.util.Lazy;
import kiln.util.StringLiteral;
import kiln.wsi.Context;
import kiln.wsi.vulkan_instance_extensions;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

auto PresentationPluginInjection::operator()(
    vulkan::InstancePlugin& instance_plugin,
    const wsi::Plugin&      wsi_plugin,
    DevicePlugin&           device_plugin
) -> PresentationPlugin
{
    for (const char* extension_name :
         wsi::vulkan_instance_extensions(wsi_plugin.context())
             .value_or(
                 util::Lazy{
                     [] -> std::span<const char* const>
                     {
                         throw PresentationPluginFailedError{
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

    // TODO: use promoted VK_KHR_surface_maintenance1 and VK_KHR_swapchain_maintenance1
    instance_plugin->enable_extension(vk::KHRGetSurfaceCapabilities2ExtensionName);
    instance_plugin->enable_extension(vk::EXTSurfaceMaintenance1ExtensionName);
    device_plugin->enable_extension(vk::EXTSwapchainMaintenance1ExtensionName);
    device_plugin->enable_features(
        vk::PhysicalDeviceSwapchainMaintenance1FeaturesEXT{
            .swapchainMaintenance1 = vk::True,
        }
    );

    return PresentationPlugin{};
}

}   // namespace kiln::gfx::renderer
