module;

#include <concepts>
#include <expected>
#include <flat_map>
#include <functional>
#include <memory_resource>
#include <ranges>
#include <utility>
#include <vector>

export module kiln.wsi.WindowProxy;

import vulkan;

import kiln.util.Bool;
import kiln.util.StringLiteral;
import kiln.wsi.Context;
import kiln.wsi.CursorMode;
import kiln.wsi.event.Key;
import kiln.wsi.event.KeyAction;
import kiln.wsi.event.Event;
import kiln.wsi.Position;
import kiln.wsi.Size;
import kiln.wsi.WindowCommand;
import kiln.wsi.WindowHandle;
import kiln.wsi.WindowSettings;

namespace kiln::wsi {

export class WindowProxy {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;

private:
    using WindowCommandContainer = std::pmr::vector<WindowCommand>;

public:
    WindowProxy(const WindowProxy&) = delete;
    WindowProxy(WindowProxy&&)      = default;
    WindowProxy(WindowProxy&&, const allocator_type&);

    explicit WindowProxy(const Context& context, WindowHandle window_handle);
    explicit WindowProxy(
        std::allocator_arg_t,
        const allocator_type& allocator,
        const Context&        context,
        WindowHandle          window_handle
    );

    auto operator=(WindowProxy&&) -> WindowProxy& = default;

    [[nodiscard]]
    auto operator==(WindowHandle window_handle) const noexcept -> bool;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto number_of_pending_changes() const noexcept -> std::size_t;

    [[nodiscard]]
    auto should_close() const noexcept -> bool;
    [[nodiscard]]
    auto framebuffer_size() const noexcept -> const Size2u&;
    [[nodiscard]]
    auto is_key_pressed(Key key) const noexcept -> bool;
    [[nodiscard]]
    auto cursor_position() const noexcept -> const Position2d&;


    auto set_title(std::string_view title) -> void;
    auto set_cursor_mode(CursorMode cursor_mode) -> void;

    [[nodiscard]]
    auto create_vulkan_surface(const vk::raii::Instance& instance)
        -> std::expected<vk::raii::SurfaceKHR, vk::Result>;

    auto destroy() -> void;

    auto update(const Event& event) -> void;
    template <typename F>
    auto flush_changes(F&& callback) -> void
        requires std::invocable<
            F,
            std::ranges::as_rvalue_view<std::ranges::ref_view<WindowCommandContainer>>>;

private:
    struct CachedState {
        bool   close_flag{};
        Size2u framebuffer_size{};
        std::flat_map<
            Key,
            util::Bool,
            std::less<>,
            std::pmr::vector<Key>,
            std::pmr::vector<util::Bool>>
                   keys;
        Position2d cursor_position{};
    };

    WindowHandle           m_handle;
    CachedState            m_cached_state;
    bool                   m_destroyed{};
    WindowCommandContainer m_changes;
};

}   // namespace kiln::wsi

namespace kiln::wsi {

template <typename F>
auto WindowProxy::flush_changes(F&& callback) -> void
    requires std::invocable<
        F,
        std::ranges::as_rvalue_view<std::ranges::ref_view<WindowCommandContainer>>>
{
    std::invoke(std::forward<F>(callback), std::views::as_rvalue(m_changes));
    m_changes.clear();
}

}   // namespace kiln::wsi
