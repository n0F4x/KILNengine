export module kiln.gfx.Bundle;

import kiln.app.Builder;

namespace kiln::gfx {

export class Bundle {
public:
    static auto operator()(app::Builder& builder) -> void;
};

}   // namespace kiln::gfx
