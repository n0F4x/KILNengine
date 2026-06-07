module;

#include <variant>

export module kiln.wsi.WindowSettings;

import kiln.wsi.FullScreenWindowSettings;
import kiln.wsi.WindowedWindowSettings;

namespace kiln::wsi {

export using WindowSettings
    = std::variant<WindowedWindowSettings, FullScreenWindowSettings>;

}   // namespace kiln::wsi
