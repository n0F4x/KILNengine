module kiln.gfx.asset.gltf.Bundle;

import kiln.gfx.asset.gltf.Parser;

namespace kiln::gfx::asset::gltf {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.use_context<Parser>();
}

}   // namespace kiln::gfx::asset::gltf
