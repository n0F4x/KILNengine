module;

#include <cassert>
#include <optional>
#include <span>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"

module kiln.wsi.monitor_functions;

import kiln.util.contracts;
import kiln.wsi.MonitorHandle;
import kiln.wsi.Size;

namespace kiln::wsi {

auto primary_monitor(const Context&) -> std::optional<MonitorHandle>
{
    GLFWmonitor* monitor{ glfwGetPrimaryMonitor() };
    if (monitor == nullptr)
    {
        return std::nullopt;
    }

    return MonitorHandle{ monitor };
}

auto active_monitors(const Context&) -> std::span<MonitorHandle>
{
    int           count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    static_assert(sizeof(MonitorHandle) == sizeof(GLFWmonitor*));
    static_assert(std::is_standard_layout_v<MonitorHandle>);

    return std::span{
        reinterpret_cast<MonitorHandle*>(monitors),
        static_cast<unsigned>(count),
    };
}

auto video_mode_of(const MonitorHandle handle) -> VideoMode
{
    PRECOND(handle != nullptr);

    const GLFWvidmode* video_mode = glfwGetVideoMode(handle.get());

    assert(
        video_mode != nullptr
        && "The registered error handler callback should have handled this error"
    );

    return VideoMode{
        .resolution{ .width  = static_cast<uint32_t>(video_mode->width),
                    .height = static_cast<uint32_t>(video_mode->height) },
        .red_bits_depth   = static_cast<uint32_t>(video_mode->redBits),
        .green_bits_depth = static_cast<uint32_t>(video_mode->greenBits),
        .blue_bits_depth  = static_cast<uint32_t>(video_mode->blueBits),
        .refresh_rate     = static_cast<uint32_t>(video_mode->refreshRate),
    };
}

}   // namespace kiln::wsi
