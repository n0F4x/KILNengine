module;

#include <utility>

export module kiln.app.config.ConfigPlugin;

import kiln.app.plugin.PluginInterface;
import kiln.app.config.Config;
import kiln.config.Version;
import kiln.util.StringLiteral;

namespace kiln::app {

export class ConfigPlugin : public PluginInterface {
public:
    constexpr explicit ConfigPlugin(const Config& config) : m_config{ config } {}

    [[nodiscard]]
    constexpr auto config() const noexcept -> const Config&
    {
        return m_config;
    }

    [[nodiscard]]
    constexpr auto operator()() && noexcept -> Config
    {
        return std::move(m_config);
    }

private:
    Config m_config;
};

}   // namespace kiln::app
