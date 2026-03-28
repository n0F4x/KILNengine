module kiln.gfx.vulkan.Bundle;

import kiln.gfx.vulkan.InstancePluginInjection;
import kiln.gfx.vulkan.DebugMessengerPluginInjection;

namespace kiln::gfx::vulkan {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.inject_plugin(InstancePluginInjection{});
    builder.inject_plugin(DebugMessengerPluginInjection{});
}

}   // namespace kiln::gfx::vulkan
