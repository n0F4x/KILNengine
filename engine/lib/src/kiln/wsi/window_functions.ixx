module;

#include <expected>

#include <GLFW/glfw3.h>

export module kiln.wsi.window_functions;

import vulkan_hpp;

import kiln.wsi.Context;
import kiln.wsi.event.Key;
import kiln.wsi.event.KeyAction;
import kiln.wsi.Size;
import kiln.wsi.WindowSettings;

namespace kiln::wsi {

export [[nodiscard]]
auto create_window(const Context&, const char* title, const WindowSettings& settings)
    -> GLFWwindow*;
export auto destroy_window(const Context&, GLFWwindow* window) -> void;

export [[nodiscard]]
auto        should_close(const Context&, GLFWwindow& window) noexcept -> bool;
export auto set_should_close_flag(const Context&, GLFWwindow& window, bool flag) noexcept
    -> void;

export [[nodiscard]]
auto content_size_of(const Context&, GLFWwindow& window) -> Size2u;
export [[nodiscard]]
auto framebuffer_size_of(const Context&, GLFWwindow& window) -> Size2u;

export [[nodiscard]]
auto get_key(const Context&, GLFWwindow& window, Key key) -> KeyAction;

export [[nodiscard]]
auto create_vulkan_surface(
    const Context&,
    GLFWwindow&               window,
    const vk::raii::Instance& instance
) -> std::expected<vk::raii::SurfaceKHR, vk::Result>;

}   // namespace kiln::wsi
