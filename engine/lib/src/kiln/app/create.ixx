module;

#include <utility>

export module kiln.app.create;

import kiln.app.Builder;
import kiln.app.config.Config;
import kiln.config.Version;
import kiln.reg.configuration_entry_c;
import kiln.util.StringLiteral;

namespace kiln::app {

export [[nodiscard]]
inline auto create(
    const util::StringLiteral app_name    = "",
    const config::Version&    app_version = {}
) -> Builder
{
    return Builder{
        Config{ app_name, app_version }
    };
}

export template <reg::decays_to_configuration_entry_c... Entries_T>
[[nodiscard]]
auto create(const Config& config, Entries_T&&... entries) -> Builder
{
    return Builder{ config, std::forward<Entries_T>(entries)... };
}

}   // namespace kiln::app
