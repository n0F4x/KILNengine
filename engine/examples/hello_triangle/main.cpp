#include <print>

import kiln;

auto main() -> int
{
    kiln::app::App app =   //
        kiln::app::create()
            .insert_plugin(kiln::config::Plugin{ "Hello triangle!" })
            .inject_plugin(kiln::gfx::vulkan::InstancePluginInjection{})
            .inject_plugin(kiln::gfx::renderer::PluginInjection{})
            .build();

    std::println(
        "Created renderer using {}",
        app.resources().at<kiln::gfx::renderer::Device>().name()
    );
}
