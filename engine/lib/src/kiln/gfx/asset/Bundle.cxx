module kiln.gfx.asset.Bundle;

import kiln.gfx.asset.gltf.Bundle;

namespace kiln::gfx::asset {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.apply_bundle(gltf::Bundle{});
}

}   // namespace kiln::gfx::asset
