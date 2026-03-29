module kiln.gfx.vulkan.Bundle;

import kiln.gfx.vulkan.InstancePlugin;
import kiln.gfx.vulkan.DebugMessengerPlugin;

namespace kiln::gfx::vulkan {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.inject_plugin(make_instance_plugin);
    builder.inject_plugin(make_debug_messenger_plugin);
}

}   // namespace kiln::gfx::vulkan
