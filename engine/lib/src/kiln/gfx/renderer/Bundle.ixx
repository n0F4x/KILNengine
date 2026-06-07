export module kiln.gfx.renderer.Bundle;

import kiln.app.Builder;

namespace kiln::gfx::renderer {

export class Bundle {
public:
    struct CreateInfo {
        bool headless{ true };
    };

    Bundle() = default;
    explicit Bundle(const CreateInfo& create_info);

    auto operator()(app::Builder& builder) const -> void;

private:
    bool m_headless;
};

}   // namespace kiln::gfx::renderer
