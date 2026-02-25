#include <print>

#include <vulkan/vulkan_raii.hpp>

import kiln;

auto game_loop(kiln::app::App& app) -> void;

auto main() -> int
{
    kiln::app::App app =   //
        kiln::app::create()
            .insert_plugin(kiln::config::Plugin{ "Hello triangle!" })
            .inject_plugin(kiln::gfx::vulkan::InstancePluginInjection{})
            .insert_plugin(kiln::wsi::Plugin{})
            .apply_bundle(kiln::gfx::renderer::Bundle{})
            .build();

    std::println(
        "Created renderer using {}",
        app.resources().at<kiln::gfx::renderer::Device>().name()
    );

    game_loop(app);
}

[[nodiscard]]
auto create_window(kiln::app::App& app) -> kiln::wsi::VulkanWindow
{
    constexpr kiln::wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    const kiln::wsi::Window::CreateInfo window_info{
        .title    = app.resources().at<kiln::config::Config>().app_name(),
        .settings = screen_settings,
    };

    const vk::raii::Instance& vulkan_instance{ app.resources().at<vk::raii::Instance>() };
    const kiln::wsi::Context& wsi_context{ app.resources().at<kiln::wsi::Context>() };
    const kiln::gfx::renderer::Device& render_device{
        app.resources().at<kiln::gfx::renderer::Device>()
    };

    return render_device.create_window(vulkan_instance, wsi_context, window_info);
}

auto game_loop(kiln::app::App& app) -> void
{
    const kiln::wsi::Context& wsi_context{ app.resources().at<kiln::wsi::Context>() };

    kiln::wsi::VulkanWindow window{ create_window(app) };

    while (!window.should_close())
    {
        kiln::wsi::poll_events(wsi_context);

        if (window.key_pressed(kiln::wsi::Key::eEscape))
        {
            window.request_close();
        }
    }
}
