export module kiln.gfx.renderer.Bundle;

import kiln.app.Builder;
import kiln.gfx.renderer.DevicePluginInjection;

namespace kiln::gfx::renderer {

export class Bundle {
public:
    Bundle() = default;
    explicit Bundle(bool headless);

    auto operator()(app::Builder& builder) const -> void;

private:
    bool m_headless;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

Bundle::Bundle(const bool headless) : m_headless{ headless } {}

auto Bundle::operator()(app::Builder& builder) const -> void
{
    builder.inject_plugin(DevicePluginInjection{ m_headless });
}

}   // namespace kiln::gfx::renderer
