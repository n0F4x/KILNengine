module;

#include <cassert>
#include <optional>
#include <span>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"

export module kiln.wsi.monitor;

import kiln.util.contracts;
import kiln.wsi.Context;
import kiln.wsi.error.Error;
import kiln.wsi.Size;

namespace kiln::wsi {

export class Monitor;

export [[nodiscard]]
auto primary_monitor(const Context&) -> std::optional<Monitor>;

export [[nodiscard]]
auto active_monitors(const Context&) -> std::span<Monitor>;

export class Monitor {
public:
    friend auto primary_monitor(const Context&) -> std::optional<Monitor>;
    friend auto active_monitors(const Context&) -> std::span<Monitor>;

    explicit operator GLFWmonitor*() const;

    [[nodiscard]]
    auto size() const -> Size2i;

private:
    explicit Monitor(GLFWmonitor* handle);

    GLFWmonitor* m_handle{};
};

}   // namespace kiln::wsi

namespace kiln::wsi {

auto primary_monitor(const Context&) -> std::optional<Monitor>
{
    GLFWmonitor* monitor{ glfwGetPrimaryMonitor() };
    if (monitor == nullptr)
    {
        return std::nullopt;
    }
    return Monitor{ monitor };
}

auto active_monitors(const Context&) -> std::span<Monitor>
{
    int           count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    static_assert(sizeof(Monitor) == sizeof(GLFWmonitor*));

    return std::span{
        reinterpret_cast<Monitor*>(monitors),
        static_cast<unsigned>(count)   //
    };
}

Monitor::operator GLFWmonitor*() const
{
    return m_handle;
}

auto Monitor::size() const -> Size2i
{
    const GLFWvidmode* video_mode = glfwGetVideoMode(m_handle);

    assert(
        video_mode != nullptr
        && "The registered error handler callback should have handled this error"
    );

    return Size2i{
        .width  = video_mode->width,
        .height = video_mode->height,
    };
}

Monitor::Monitor(GLFWmonitor* const handle) : m_handle{ handle }
{
    PRECOND(m_handle != nullptr);
}

}   // namespace kiln::wsi
