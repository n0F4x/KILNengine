module examples.simple_scene.Window;

import kiln.wsi.WindowedWindowSettings;

namespace demo {

[[nodiscard]]
auto create_window(const kiln::app::Config& config, const kiln::wsi::Context& wsi_context)
    -> kiln::wsi::Window
{
    constexpr static kiln::wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    const kiln::wsi::Window::CreateInfo window_info{
        .title    = config.app_name(),
        .settings = screen_settings,
    };

    return kiln::wsi::Window{ wsi_context, window_info };
}

Window::Window(const kiln::app::Config& config, const kiln::wsi::Context& context)
    : m_window{ create_window(config, context) }
{
}

auto Window::operator*() -> kiln::wsi::Window&
{
    return m_window;
}

auto Window::operator*() const -> const kiln::wsi::Window&
{
    return m_window;
}

auto Window::operator->() -> kiln::wsi::Window*
{
    return &m_window;
}

auto Window::operator->() const -> const kiln::wsi::Window*
{
    return &m_window;
}

auto Window::Builder::build(
    const kiln::app::Config&  config,
    const kiln::wsi::Context& context
) -> Window
{
    return Window{ config, context };
}

}   // namespace demo
