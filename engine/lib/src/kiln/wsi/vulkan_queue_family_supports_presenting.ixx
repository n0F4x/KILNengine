module;

#include <cassert>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"

export module kiln.wsi.vulkan_queue_family_supports_presenting;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.wsi.Context;
import kiln.wsi.Error;
import kiln.util.contracts;

namespace kiln::wsi {

export [[nodiscard]]
auto vulkan_queue_family_supports_presenting(
    const Context&,
    const vk::raii::Instance&           instance,
    const vk::raii::PhysicalDevice&     physical_device,
    const gfx::vulkan::QueueFamilyIndex queue_family_index
) -> bool
{
    const int result{
        glfwGetPhysicalDevicePresentationSupport(
            *instance,
            *physical_device,
            queue_family_index.underlying()
        )   //
    };
    if (result != GLFW_TRUE)
    {
        const char* error_description{};
        const int   error_code = glfwGetError(&error_description);
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        PRECOND(error_code != GLFW_API_UNAVAILABLE && "Vulkan support is not available");
        if (error_code == GLFW_PLATFORM_ERROR)
        {
            throw Error{ error_description };
        }
        assert(error_code == GLFW_NO_ERROR && "Other error codes are unspecified");
    }

    return result == GLFW_TRUE;
}

}   // namespace kiln::wsi
