#include <print>

import kiln;
import demo;

auto run(kiln::app::App& app) -> void;

auto main() -> int
{
    kiln::app::App app =   //
        kiln::app::create()
            .insert_plugin(kiln::config::Plugin{ "Hello triangle!" })
            .inject_plugin(kiln::gfx::vulkan::InstancePluginInjection{})
            .insert_plugin(kiln::wsi::Plugin{})
            .apply_bundle(kiln::gfx::renderer::Bundle{})
            .inject_plugin(demo_plugin_injection)
            .build();

    std::println(
        "Created renderer using {}",
        app.resources().at<kiln::gfx::renderer::Device>().name()
    );

    run(app);
}

auto run(kiln::app::App& app) -> void
{
    const kiln::wsi::Context& wsi_context{ app.resources().at<kiln::wsi::Context>() };
    Demo&                     demo{ app.resources().at<Demo>() };

    while (!demo.window.should_close())
    {
        kiln::wsi::poll_events(wsi_context);

        if (demo.window.key_pressed(kiln::wsi::Key::eEscape))
        {
            demo.window.request_close();
        }
    }
}
