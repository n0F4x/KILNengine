module kiln.gfx.vulkan.Bundle;

import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.DebugMessenger;

namespace kiln::gfx::vulkan {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.register_entry<Instance>();
    builder.register_entry<DebugMessenger>();
}

}   // namespace kiln::gfx::vulkan
