module kiln.gfx.vulkan.Bundle;

import kiln.gfx.vulkan.InstancePlugin;
import kiln.gfx.vulkan.DebugMessengerPlugin;

namespace kiln::gfx::vulkan {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.use_plugin<InstancePlugin>();
    builder.use_plugin<DebugMessengerPlugin>();
}

}   // namespace kiln::gfx::vulkan
