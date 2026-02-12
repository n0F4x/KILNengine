module;

#include <cstdint>

export module kiln.config.engine_version;

import kiln.config.Version;

namespace kiln::config {

export [[nodiscard]]
constexpr auto engine_version() noexcept -> const Version&
{
    constexpr static Version result{
        .major = KILN_VERSION_MAJOR,
        .minor = KILN_VERSION_MINOR,
        .patch = KILN_VERSION_PATCH,
    };

    return result;
}

}   // namespace kiln::config
