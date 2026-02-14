module;

#include <cassert>
#include <cstddef>
#include <optional>
#include <span>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"

export module kiln.wsi.monitor;

import kiln.wsi.Context;
import kiln.wsi.Error;
import kiln.wsi.Size;
import kiln.util.contracts;

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

    return std::span{
        reinterpret_cast<Monitor*>(monitors),
        static_cast<std::size_t>(count)   //
    };
}

Monitor::operator GLFWmonitor*() const
{
    return m_handle;
}

auto Monitor::size() const -> Size2i
{
    Size2i result;

    const GLFWvidmode* video_mode = glfwGetVideoMode(m_handle);
    if (video_mode == nullptr)
    {
        const char* error_description{};
        [[maybe_unused]]
        const int error_code = glfwGetError(&error_description);
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        assert(error_code == GLFW_PLATFORM_ERROR && "Other error codes are unspecified");

        throw Error{ error_description };
    }

    result.width  = video_mode->width;
    result.height = video_mode->height;

    return result;
}

Monitor::Monitor(GLFWmonitor* const handle) : m_handle{ handle }
{
    PRECOND(m_handle != nullptr);
}

}   // namespace kiln::wsi
