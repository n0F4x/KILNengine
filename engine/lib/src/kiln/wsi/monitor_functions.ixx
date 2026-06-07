module;

#include <optional>
#include <span>

export module kiln.wsi.monitor_functions;

import kiln.wsi.Context;
import kiln.wsi.MonitorHandle;
import kiln.wsi.VideoMode;

namespace kiln::wsi {

export [[nodiscard]]
auto primary_monitor(const Context&) -> std::optional<MonitorHandle>;

export [[nodiscard]]
auto active_monitors(const Context&) -> std::span<MonitorHandle>;

export [[nodiscard]]
auto video_mode_of(MonitorHandle handle) -> VideoMode;

}   // namespace kiln::wsi
