#pragma once

#include <cstdint>

namespace kiln::config {

struct EngineVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

[[nodiscard]]
constexpr auto engine_version() noexcept -> const EngineVersion&
{
    constexpr static EngineVersion result{
        .major = KILN_VERSION_MAJOR,
        .minor = KILN_VERSION_MINOR,
        .patch = KILN_VERSION_PATCH,
    };

    return result;
}

}   // namespace kiln::config
