#include <print>

import kiln;

auto game_loop(kiln::app::App& app) -> void;

auto main() -> int
{
    kiln::app::App app =   //
        kiln::app::create()
            .insert_plugin(kiln::config::Plugin{ "Hello triangle!" })
            .inject_plugin(kiln::gfx::vulkan::InstancePluginInjection{})
            .insert_plugin(kiln::wsi::Plugin{})
            .inject_plugin(kiln::gfx::renderer::PluginInjection{})
            .build();

    std::println(
        "Created renderer using {}",
        app.resources().at<kiln::gfx::renderer::Device>().name()
    );

    game_loop(app);
}

auto game_loop(kiln::app::App& app) -> void
{
    constexpr kiln::wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    const kiln::wsi::Window::CreateInfo window_info{
        .title    = app.resources().at<kiln::config::Config>().app_name(),
        .settings = screen_settings,
    };

    const kiln::wsi::Context& wsi_context{ app.resources().at<kiln::wsi::Context>() };
    kiln::wsi::Window         window{ wsi_context, window_info };

    while (!window.should_close())
    {
        kiln::wsi::poll_events(wsi_context);

        if (window.key_pressed(kiln::wsi::Key::eEscape))
        {
            window.request_close();
        }
    }
}
