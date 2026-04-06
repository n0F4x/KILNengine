module kiln.gfx.vulkan.Bundle;

import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.DebugMessenger;

namespace kiln::gfx::vulkan {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.use_context<Instance>();
    builder.use_context<DebugMessenger>();
}

}   // namespace kiln::gfx::vulkan
