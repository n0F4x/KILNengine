module;

#include <cstdint>
#include <optional>

export module kiln.wsi.FullScreenWindowSettings;

import kiln.wsi.monitor;

namespace kiln::wsi {

export struct FullScreenWindowSettings {
    Monitor                 monitor;
    bool                    auto_iconify{ true };
    bool                    center_cursor{ true };
    std::optional<uint16_t> refresh_rate;
};

}   // namespace kiln::wsi
