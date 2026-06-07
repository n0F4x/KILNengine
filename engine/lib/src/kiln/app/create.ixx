export module kiln.app.create;

import kiln.app.Builder;
import kiln.app.config.Config;
import kiln.config.Version;
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

}   // namespace kiln::app
