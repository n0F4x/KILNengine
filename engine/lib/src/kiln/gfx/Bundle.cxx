module kiln.gfx.Bundle;

import kiln.gfx.model.ModelBundle;
import kiln.gfx.renderer.Bundle;
import kiln.gfx.vulkan.Bundle;

namespace kiln::gfx {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.apply_bundle(vulkan::Bundle{});
    builder.apply_bundle(renderer::Bundle{});
    builder.apply_bundle(ModelBundle{});
}

}   // namespace kiln::gfx
