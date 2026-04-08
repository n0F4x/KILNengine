module kiln.gfx.model.ModelBundle;

import kiln.gfx.model.GltfLoader;

namespace kiln::gfx {

auto ModelBundle::operator()(app::Builder& builder) -> void
{
    builder.use_context<GltfLoader>();
}

}   // namespace kiln::gfx
