module;

#include <utility>

export module kiln.app.config.Config;

import kiln.app.context.context_c;
import kiln.app.context.ContextBase;
import kiln.app.context.ContextBuilderInterface;
import kiln.config.engine_name;
import kiln.config.engine_version;
import kiln.config.Version;
import kiln.util.StringLiteral;

namespace kiln::app {

namespace internal {

export class ConfigBuilder;

}   // namespace internal

export class Config : public ContextBase {
public:
    using Builder = internal::ConfigBuilder;

    constexpr explicit Config(
        const util::StringLiteral app_name    = "",
        const config::Version&    app_version = {}
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
    constexpr static auto engine_version() -> const config::Version&
    {
        return config::engine_version();
    }

    [[nodiscard]]
    constexpr auto app_name() const -> util::StringLiteral
    {
        return m_app_name;
    }

    [[nodiscard]]
    constexpr auto app_version() const -> const config::Version&
    {
        return m_app_version;
    }

private:
    util::StringLiteral m_app_name{ "" };
    config::Version     m_app_version{};
};

namespace internal {

export class ConfigBuilder : public ContextBuilderInterface {
public:
    constexpr explicit ConfigBuilder(const Config& config = Config{}) : m_config{ config }
    {
    }

    [[nodiscard]]
    constexpr auto config() const noexcept -> const Config&
    {
        return m_config;
    }

    [[nodiscard]]
    constexpr auto build() && noexcept -> Config
    {
        return std::move(m_config);
    }

private:
    Config m_config;
};

}   // namespace internal

}   // namespace kiln::app
