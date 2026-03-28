export module kiln.gfx.Bundle;

import kiln.app.Builder;
import kiln.gfx.renderer.Bundle;
import kiln.gfx.vulkan.Bundle;

namespace kiln::gfx {

export class Bundle {
public:
    static auto operator()(app::Builder& builder) -> void
    {
        builder.apply_bundle(vulkan::Bundle{});
        builder.apply_bundle(renderer::Bundle{});
    }
};

}   // namespace kiln::gfx
