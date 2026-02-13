import kiln;

auto main() -> int
{
    [[maybe_unused]]
    auto app = kiln::app::Builder{}
                   .insert_plugin(kiln::config::Plugin{ "Hello triangle!" })
                   .inject_plugin(kiln::gfx::vulkan::InstancePluginInjection{})
                   .inject_plugin(kiln::gfx::renderer::PluginInjection{})
                   .build();
}
