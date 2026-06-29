module;

#include <expected>

export module kiln.wsi.Window;

import vulkan;

import kiln.util.StringLiteral;
import kiln.wsi.Context;
import kiln.wsi.event.Key;
import kiln.wsi.Size;
import kiln.wsi.WindowHandle;
import kiln.wsi.WindowSettings;

namespace kiln::wsi {

export class Window {
public:
    struct CreateInfo {
        util::StringLiteral title{ "" };
        WindowSettings      settings;
    };

    Window(const Window&) = delete;
    Window(Window&&) noexcept;
    ~Window();

    explicit Window(const Context& context, const CreateInfo& create_info);

    auto operator=(const Window&) -> Window& = delete;
    auto operator=(Window&&) noexcept -> Window&;


    [[nodiscard]]
    auto context() const noexcept -> const Context&;

    [[nodiscard]]
    auto content_size() const noexcept -> Size2u;
    [[nodiscard]]
    auto framebuffer_size() const noexcept -> Size2u;

    [[nodiscard]]
    auto should_close() const noexcept -> bool;
    [[nodiscard]]
    auto key_pressed(Key key) const noexcept -> bool;

    auto request_close() noexcept -> void;

    [[nodiscard]]
    auto create_vulkan_surface(const vk::raii::Instance& instance)
        -> std::expected<vk::raii::SurfaceKHR, vk::Result>;

private:
    Context      m_context;
    WindowHandle m_handle;


    auto reset() -> void;
};

}   // namespace kiln::wsi
