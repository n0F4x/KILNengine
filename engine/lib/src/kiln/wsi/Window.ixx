module;

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <variant>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"
#include "kiln/util/lifetimebound.hpp"

export module kiln.wsi.Window;

import kiln.wsi.Context;
import kiln.wsi.event.Key;
import kiln.wsi.Error;
import kiln.wsi.FullScreenWindowSettings;
import kiln.wsi.monitor;
import kiln.wsi.Size;
import kiln.wsi.WindowSettings;
import kiln.wsi.WindowedWindowSettings;
import kiln.util.contracts;
import kiln.util.Overloaded;
import kiln.util.StringLiteral;

namespace kiln::wsi {

export class Window {
public:
    struct CreateInfo {
        util::StringLiteral title{ "" };
        WindowSettings      settings;
    };

    Window(
        [[kiln_lifetimebound]]
        const Context&    context,
        const CreateInfo& create_info
    );

    [[nodiscard]]
    auto content_size() const noexcept -> Size2i;
    [[nodiscard]]
    auto resolution() const noexcept -> Size2i;

    [[nodiscard]]
    auto should_close() const noexcept -> bool;
    [[nodiscard]]
    auto key_pressed(Key key) const noexcept -> bool;

    auto request_close() noexcept -> void;

protected:
    [[nodiscard]]
    auto context() const noexcept -> const Context&;

    [[nodiscard]]
    auto handle() noexcept -> GLFWwindow&;
    [[nodiscard]]
    auto handle() const noexcept -> const GLFWwindow&;

private:
    std::reference_wrapper<const Context>                     m_context_ref;
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_handle;
};

}   // namespace kiln::wsi

module :private;

namespace kiln::wsi {

auto destroy_window(GLFWwindow* const handle) -> void
{
    if (handle == nullptr)
    {
        return;
    }

    glfwDestroyWindow(handle);

    [[maybe_unused]]
    const int error_code = glfwGetError(nullptr);
    PRECOND(error_code != GLFW_NOT_INITIALIZED);
    assert(
        error_code != GLFW_PLATFORM_ERROR
        && "A bug or configuration error in GLFW,"
           " the underlying operating system or its drivers,"
           " or a lack of required resources "
           "- Issue a ticket to GLFW"
    );
    assert(error_code == GLFW_NO_ERROR && "Other error codes are unspecified");
}

[[nodiscard]]
auto create_window(const Window::CreateInfo& create_info)
    -> std::unique_ptr<GLFWwindow, decltype(&destroy_window)>
{
    const auto [width, height]{
        std::visit(
            util::Overloaded{
                [](const WindowedWindowSettings& settings) static -> Size2i
                { return settings.content_size; },
                [](const FullScreenWindowSettings& settings) static -> Size2i
                { return settings.monitor.size(); },
            },
            create_info.settings
        )   //
    };

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (const WindowedWindowSettings* settings{
            std::get_if<WindowedWindowSettings>(&create_info.settings) };
        settings != nullptr)
    {
        glfwWindowHint(GLFW_POSITION_X, settings->position_x.value_or(GLFW_ANY_POSITION));
        glfwWindowHint(GLFW_POSITION_Y, settings->position_y.value_or(GLFW_ANY_POSITION));

        const std::optional<WindowedWindowSettings::Border> border{
            settings->border   //
        };
        glfwWindowHint(GLFW_DECORATED, border.has_value());
        if (border.has_value())
        {
            glfwWindowHint(GLFW_RESIZABLE, border->resizable);
        }

        switch (settings->visibility)
        {
            case WindowedWindowSettings::Visibility::eDefault:
                glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
                glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
                break;
            case WindowedWindowSettings::Visibility::eHidden:
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
                break;
            case WindowedWindowSettings::Visibility::eFocused:
                glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
                glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
                break;
        }

        glfwWindowHint(GLFW_MAXIMIZED, settings->maximized);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, settings->focus_on_show);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, settings->scale_to_monitor);
    }
    if (const FullScreenWindowSettings* settings{
            std::get_if<FullScreenWindowSettings>(&create_info.settings) };
        settings != nullptr)
    {
        glfwWindowHint(GLFW_AUTO_ICONIFY, settings->auto_iconify);
        glfwWindowHint(GLFW_CENTER_CURSOR, settings->center_cursor);
        glfwWindowHint(GLFW_REFRESH_RATE, settings->refresh_rate.value_or(GLFW_DONT_CARE));
    }

    GLFWwindow* const window = glfwCreateWindow(
        width,
        height,
        create_info.title,
        std::visit(
            util::Overloaded{
                [](const WindowedWindowSettings&) static -> GLFWmonitor*
                { return nullptr; },
                [](const FullScreenWindowSettings& settings) static -> GLFWmonitor*
                { return static_cast<GLFWmonitor*>(settings.monitor); },
            },
            create_info.settings
        ),
        nullptr
    );

    if (window == nullptr)
    {
        const char* error_description{};
        [[maybe_unused]]
        const int error_code = glfwGetError(&error_description);
        assert(error_code != GLFW_NO_ERROR);
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        PRECOND(error_code != GLFW_INVALID_ENUM);
        PRECOND(error_code != GLFW_INVALID_VALUE);
        PRECOND(error_code != GLFW_API_UNAVAILABLE && "Vulkan support is not available");
        PRECOND(error_code != GLFW_VERSION_UNAVAILABLE);
        PRECOND(error_code != GLFW_FORMAT_UNAVAILABLE);
        PRECOND(error_code != GLFW_NO_WINDOW_CONTEXT);
        assert(error_code == GLFW_PLATFORM_ERROR && "Other error codes are unspecified");

        throw Error{ error_description };
    }

    return std::unique_ptr<GLFWwindow, decltype(&destroy_window)>{
        window,
        destroy_window   //
    };
}

Window::Window(const Context& context, const CreateInfo& create_info)
    : m_context_ref{ context },
      m_handle{ create_window(create_info) }
{
}

auto Window::content_size() const noexcept -> Size2i
{
    Size2i result{};
    glfwGetWindowSize(m_handle.get(), &result.width, &result.height);
    return result;
}

auto Window::resolution() const noexcept -> Size2i
{
    Size2i result{};
    glfwGetFramebufferSize(m_handle.get(), &result.width, &result.height);
    return result;
}

auto Window::should_close() const noexcept -> bool
{
    return glfwWindowShouldClose(m_handle.get()) == GLFW_TRUE;
}

auto Window::key_pressed(const Key key) const noexcept -> bool
{
    return glfwGetKey(m_handle.get(), std::to_underlying(key)) == GLFW_PRESS;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Window::request_close() noexcept -> void
{
    glfwSetWindowShouldClose(m_handle.get(), GLFW_TRUE);
}

auto Window::context() const noexcept -> const Context&
{
    return m_context_ref;
}

auto Window::handle() noexcept -> GLFWwindow&
{
    return *m_handle;
}

auto Window::handle() const noexcept -> const GLFWwindow&
{
    return *m_handle;
}

}   // namespace kiln::wsi
