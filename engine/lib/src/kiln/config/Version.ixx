module;

#include <cstdint>

export module kiln.config.Version;

namespace kiln::config {

export struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

}   // namespace kiln::config
