module;

#include <cassert>
#include <expected>
#include <optional>
#include <variant>

#include <GLFW/glfw3.h>

module kiln.wsi.window_functions;

import vulkan_hpp;

import kiln.util.Overloaded;
import kiln.wsi.FullScreenWindowSettings;
import kiln.wsi.Size;
import kiln.wsi.WindowedWindowSettings;

namespace kiln::wsi {

auto create_window(const Context&, const char* const title, const WindowSettings& settings)
    -> GLFWwindow*
{
    const auto [width, height]{
        std::visit(
            util::Overloaded{
                [](const WindowedWindowSettings& windowed_settings) static -> Size2i
                {
                    return windowed_settings.content_size;   //
                },
                [](const FullScreenWindowSettings& full_screen_settings) static -> Size2i
                {
                    return full_screen_settings.monitor.size();   //
                },
            },
            settings
        ),
    };

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (const WindowedWindowSettings* windowed_settings{
            std::get_if<WindowedWindowSettings>(&settings) };
        windowed_settings != nullptr)
    {
        glfwWindowHint(
            GLFW_POSITION_X,
            windowed_settings->position_x.value_or(GLFW_ANY_POSITION)
        );
        glfwWindowHint(
            GLFW_POSITION_Y,
            windowed_settings->position_y.value_or(GLFW_ANY_POSITION)
        );

        const std::optional<WindowedWindowSettings::Border> border{
            windowed_settings->border
        };
        glfwWindowHint(GLFW_DECORATED, border.has_value());
        if (border.has_value())
        {
            glfwWindowHint(GLFW_RESIZABLE, border->resizable);
        }

        switch (windowed_settings->visibility)
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

        glfwWindowHint(GLFW_MAXIMIZED, windowed_settings->maximized);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, windowed_settings->focus_on_show);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, windowed_settings->scale_to_monitor);
    }
    if (const FullScreenWindowSettings* full_screen_settings{
            std::get_if<FullScreenWindowSettings>(&settings) };
        full_screen_settings != nullptr)
    {
        glfwWindowHint(GLFW_AUTO_ICONIFY, full_screen_settings->auto_iconify);
        glfwWindowHint(GLFW_CENTER_CURSOR, full_screen_settings->center_cursor);
        glfwWindowHint(
            GLFW_REFRESH_RATE,
            full_screen_settings->refresh_rate.value_or(GLFW_DONT_CARE)
        );
    }

    GLFWwindow* const window = glfwCreateWindow(
        width,
        height,
        title,
        std::visit(
            util::Overloaded{
                [](const WindowedWindowSettings&) static -> GLFWmonitor*
                {
                    return nullptr;   //
                },
                [](const FullScreenWindowSettings& full_screen_settings) static
                    -> GLFWmonitor*
                {
                    return static_cast<GLFWmonitor*>(full_screen_settings.monitor);   //
                },
            },
            settings
        ),
        nullptr
    );

    assert(
        window != nullptr
        && "The registered error handler callback should have handled this error"
    );

    return window;
}

auto destroy_window(const Context&, GLFWwindow* const window) -> void
{
    glfwDestroyWindow(window);
}

auto should_close(const Context&, GLFWwindow& window) noexcept -> bool
{
    return glfwWindowShouldClose(&window) == GLFW_TRUE;
}

auto set_should_close_flag(const Context&, GLFWwindow& window, const bool flag) noexcept
    -> void
{
    glfwSetWindowShouldClose(&window, flag);
}

auto content_size_of(const Context&, GLFWwindow& window) -> Size2u
{
    int width{};
    int height{};
    glfwGetWindowSize(&window, &width, &height);

    assert(width >= 0);
    assert(height >= 0);

    return Size2u{
        .width  = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
    };
}

auto framebuffer_size_of(const Context&, GLFWwindow& window) -> Size2u
{
    int width{};
    int height{};
    glfwGetFramebufferSize(&window, &width, &height);

    assert(width >= 0);
    assert(height >= 0);

    return Size2u{
        .width  = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
    };
}

auto get_key(const Context&, GLFWwindow& window, const Key key) -> KeyAction
{
    return KeyAction{ glfwGetKey(&window, std::to_underlying(key)) };
}

auto create_vulkan_surface(
    const Context&,
    GLFWwindow&               window,
    const vk::raii::Instance& instance
) -> std::expected<vk::raii::SurfaceKHR, vk::Result>
{
    VkSurfaceKHR   surface{};
    const VkResult result
        = glfwCreateWindowSurface(*instance, &window, nullptr, &surface);

    if (result != VK_SUCCESS)
    {
        return std::expected<vk::raii::SurfaceKHR, vk::Result>{
            std::unexpect,
            vk::Result{ result },
        };
    }

    return std::expected<vk::raii::SurfaceKHR, vk::Result>{
        vk::raii::SurfaceKHR{ instance, surface }
    };
}

}   // namespace kiln::wsi
