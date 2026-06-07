module;

#include <cstdint>

export module kiln.gfx.vulkan.QueueFamilyIndex;

import kiln.util.Strong;

namespace kiln::gfx::vulkan {

export class QueueFamilyIndex : public util::Strong<uint32_t, QueueFamilyIndex> {
public:
    using Strong::Strong;
};

}   // namespace kiln::gfx::vulkan
