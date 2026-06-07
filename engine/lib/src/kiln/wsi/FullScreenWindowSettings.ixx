module;

#include <cstdint>
#include <optional>

export module kiln.wsi.FullScreenWindowSettings;

import kiln.wsi.MonitorHandle;
import kiln.wsi.Size;

namespace kiln::wsi {

export struct FullScreenWindowSettings {
    MonitorHandle           monitor;
    std::optional<Size2u>   resolution;
    std::optional<uint16_t> refresh_rate;
    bool                    auto_iconify{ true };
    bool                    center_cursor{ true };
};

}   // namespace kiln::wsi
