module;

#include <stdexcept>

export module kiln.gfx.vulkan.result.VulkanError;

import vulkan;

namespace kiln::gfx::vulkan {

struct VulkanErrorPrecondition {
    explicit VulkanErrorPrecondition(vk::Result runtime_error_code);
};

export class VulkanError : public VulkanErrorPrecondition, public std::runtime_error {
public:
    explicit VulkanError(vk::Result runtime_error_code);
};

}   // namespace kiln::gfx::vulkan
