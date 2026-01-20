module;

#include <cstdint>

export module kiln.config.engine_version;

namespace kiln::config {

export struct EngineVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

export [[nodiscard]]
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
