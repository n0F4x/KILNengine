module;

#include <cassert>
#include <expected>
#include <span>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"
#include "kiln/util/lifetimebound.hpp"

export module kiln.wsi.vulkan_instance_extensions;

import kiln.wsi.Context;
import kiln.util.contracts;

namespace kiln::wsi {

export struct VulkanSurfaceCreationNotSupportedError {};

export [[nodiscard]]
auto vulkan_instance_extensions([[kiln_lifetimebound]] const Context&)
    -> std::expected<std::span<const char* const>, VulkanSurfaceCreationNotSupportedError>
{
    uint32_t           count{};
    const char** const extension_names{ glfwGetRequiredInstanceExtensions(&count) };
    if (extension_names == nullptr)
    {
        [[maybe_unused]]
        const int error_code = glfwGetError(nullptr);
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        PRECOND(error_code != GLFW_API_UNAVAILABLE && "Vulkan support is not available");
        assert(error_code == GLFW_NO_ERROR && "Other error codes are unspecified");

        return std::unexpected{ VulkanSurfaceCreationNotSupportedError{} };
    }

    return std::span{
        extension_names,
        count,
    };
}

}   // namespace kiln::wsi
