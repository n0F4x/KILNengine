module;

#include <cstdint>

export module kiln.gfx.vulkan.Profile;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;

namespace kiln::gfx::vulkan {

export struct Profile {
    uint32_t                   api_version;
    PhysicalDeviceCapabilities physical_device_capabilities;
};

}   // namespace kiln::gfx::vulkan
