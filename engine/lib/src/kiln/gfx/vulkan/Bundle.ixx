export module kiln.gfx.vulkan.Bundle;

import kiln.app.Builder;

namespace kiln::gfx::vulkan {

export class Bundle {
public:
    static auto operator()(app::Builder& builder) -> void;
};

}   // namespace kiln::gfx::vulkan
