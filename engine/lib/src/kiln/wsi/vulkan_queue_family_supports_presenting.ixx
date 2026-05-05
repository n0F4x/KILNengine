module;

#include <GLFW/glfw3.h>

export module kiln.wsi.vulkan_queue_family_supports_presenting;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.wsi.Context;

namespace kiln::wsi {

export [[nodiscard]]
auto vulkan_queue_family_supports_presenting(
    const Context&,
    const vk::raii::Instance&           instance,
    const vk::raii::PhysicalDevice&     physical_device,
    const gfx::vulkan::QueueFamilyIndex queue_family_index
) -> bool
{
    return glfwGetPhysicalDevicePresentationSupport(
               *instance,
               *physical_device,
               queue_family_index.underlying()
           )
        == GLFW_TRUE;
}

}   // namespace kiln::wsi
