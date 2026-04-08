export module kiln.gfx.model.ModelBundle;

import kiln.app.Builder;

namespace kiln::gfx {

export class ModelBundle {
public:
    static auto operator()(app::Builder& builder) -> void;
};

}   // namespace kiln::gfx
