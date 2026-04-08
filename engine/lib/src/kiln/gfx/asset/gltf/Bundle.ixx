export module kiln.gfx.asset.gltf.Bundle;

import kiln.app.Builder;

namespace kiln::gfx::asset::gltf {

export class Bundle {
public:
    static auto operator()(app::Builder& builder) -> void;
};

}   // namespace kiln::gfx::asset::gltf
