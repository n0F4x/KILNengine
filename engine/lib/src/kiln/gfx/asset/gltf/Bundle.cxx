module kiln.gfx.asset.gltf.Bundle;

import kiln.gfx.asset.gltf.Loader;

namespace kiln::gfx::asset::gltf {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.use_context<Loader>();
}

}   // namespace kiln::gfx::asset::gltf
