export module kiln.config.Config;

import kiln.config.engine_name;
import kiln.config.engine_version;
import kiln.config.Version;
import kiln.util.StringLiteral;

namespace kiln::config {

export class Config {
public:
    constexpr explicit Config(
        const util::StringLiteral app_name,
        const Version&            app_version = {}
    )
        : m_app_name{ app_name },
          m_app_version{ app_version }
    {
    }

    [[nodiscard]]
    consteval static auto engine_name() -> util::StringLiteral
    {
        return config::engine_name();
    }

    [[nodiscard]]
    constexpr static auto engine_version() -> const Version&
    {
        return config::engine_version();
    }

    [[nodiscard]]
    constexpr auto app_name() const -> util::StringLiteral
    {
        return m_app_name;
    }

    [[nodiscard]]
    constexpr auto app_version() const -> const Version&
    {
        return m_app_version;
    }

private:
    util::StringLiteral m_app_name;
    Version             m_app_version;
};

}   // namespace kiln::config
