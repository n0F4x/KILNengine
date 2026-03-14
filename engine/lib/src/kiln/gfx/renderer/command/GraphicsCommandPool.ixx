module;

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.command.GraphicsCommandPool;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandPoolBase;
import kiln.gfx.renderer.command.CommandPoolFlags;
import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.util.EnumMask;

namespace kiln::gfx::renderer {

export class GraphicsCommandPool : public CommandPoolBase {
public:
    GraphicsCommandPool(
        [[kiln_lifetimebound]]
        const Device&                    device,
        vulkan::QueueFamilyIndex         queue_family_index,
        util::EnumMask<CommandPoolFlags> flags = CommandPoolFlags::eNone
    );

    auto allocate_primary() -> GraphicsCommandBuffer;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

GraphicsCommandPool::GraphicsCommandPool(
    const Device&                          device,
    const vulkan::QueueFamilyIndex         queue_family_index,
    const util::EnumMask<CommandPoolFlags> flags
)
    : CommandPoolBase{ device, queue_family_index, flags }
{
}

auto GraphicsCommandPool::allocate_primary() -> GraphicsCommandBuffer
{
    return GraphicsCommandBuffer{ CommandPoolBase::allocate_primary() };
}

}   // namespace kiln::gfx::renderer
