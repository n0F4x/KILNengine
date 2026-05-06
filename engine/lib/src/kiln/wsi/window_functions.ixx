module;

#include <expected>

#include <GLFW/glfw3.h>

export module kiln.wsi.window_functions;

import vulkan_hpp;

import kiln.wsi.Context;
import kiln.wsi.event.Key;
import kiln.wsi.event.KeyAction;
import kiln.wsi.Size;
import kiln.wsi.WindowHandle;
import kiln.wsi.WindowSettings;

namespace kiln::wsi {

export [[nodiscard]]
auto create_window(const Context&, const char* title, const WindowSettings& settings)
    -> WindowHandle;
export auto destroy_window(const Context&, WindowHandle window) -> void;

export [[nodiscard]]
auto        should_close(const Context&, WindowHandle window) noexcept -> bool;
export auto set_should_close_flag(const Context&, WindowHandle window, bool flag) noexcept
    -> void;

export [[nodiscard]]
auto content_size_of(const Context&, WindowHandle window) -> Size2u;
export [[nodiscard]]
auto framebuffer_size_of(const Context&, WindowHandle window) -> Size2u;

export [[nodiscard]]
auto get_key(const Context&, WindowHandle window, Key key) -> KeyAction;

export [[nodiscard]]
auto create_vulkan_surface(
    const Context&,
    WindowHandle              window,
    const vk::raii::Instance& instance
) -> std::expected<vk::raii::SurfaceKHR, vk::Result>;

}   // namespace kiln::wsi
