module;

#include <utility>

export module kiln.app.config.Config;

import kiln.config.engine_name;
import kiln.config.engine_version;
import kiln.config.Version;
import kiln.reg.EntryTraits;
import kiln.util.StringLiteral;

namespace kiln::app {

export class Config {
public:
    constexpr explicit Config(
        const util::StringLiteral app_name    = "",
        const config::Version&    app_version = {}
    )
        : m_app_name{ app_name },
          m_app_version{ app_version }
    {
    }

    [[nodiscard]]
    consteval static auto engine_name() noexcept -> util::StringLiteral
    {
        return config::engine_name();
    }

    [[nodiscard]]
    constexpr static auto engine_version() noexcept -> const config::Version&
    {
        return config::engine_version();
    }

    [[nodiscard]]
    constexpr auto app_name() const noexcept -> util::StringLiteral
    {
        return m_app_name;
    }

    [[nodiscard]]
    constexpr auto app_version() const noexcept -> const config::Version&
    {
        return m_app_version;
    }

private:
    util::StringLiteral m_app_name{ "" };
    config::Version     m_app_version{};
};

}   // namespace kiln::app

template <>
struct kiln::reg::EntryTraits<kiln::app::Config> {
    constexpr static bool is_configuration_entry{ true };
};
