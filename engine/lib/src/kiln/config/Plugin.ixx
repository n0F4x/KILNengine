export module kiln.config.Plugin;

import kiln.app.App;
import kiln.config.Config;
import kiln.config.Version;
import kiln.util.StringLiteral;

namespace kiln::config {

export class Plugin {
public:
    constexpr explicit Plugin(
        const util::StringLiteral app_name,
        const Version&            app_version = {}
    )
        : m_config{ app_name, app_version }
    {
    }

    [[nodiscard]]
    constexpr auto config() const noexcept -> const Config&
    {
        return m_config;
    }

    constexpr auto operator()(app::App& app) const -> void
    {
        app.resources().insert(m_config);
    }

private:
    Config m_config;
};

}   // namespace kiln::config
