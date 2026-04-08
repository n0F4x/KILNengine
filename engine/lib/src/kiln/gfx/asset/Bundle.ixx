export module kiln.gfx.asset.Bundle;

import kiln.app.Builder;

namespace kiln::gfx::asset {

export class Bundle {
public:
    static auto operator()(app::Builder& builder) -> void;
};

}   // namespace kiln::gfx::asset
