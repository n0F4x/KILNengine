#include <print>
#include <string_view>

#include <kiln/util/contract_macros.hpp>

import kiln;
import kiln.app;
import kiln.util.contracts;
import kiln.util.OptionalRef;

struct GraphicsSystemIntegration {};

struct GraphicsSystemIntegrationPlugin : kiln::app::PluginInterface {
    static auto build() -> GraphicsSystemIntegration
    {
        return GraphicsSystemIntegration{};
    }
};

struct WindowSystem {
    GraphicsSystemIntegration* graphics_system{};
};

struct WindowPlugin : kiln::app::PluginInterface {
    bool supports_graphics          = false;
    bool graphics_support_requested = false;

    auto build(
        const kiln::util::OptionalRef<GraphicsSystemIntegration> graphics_system_integration
    ) const -> WindowSystem
    {
        if (supports_graphics && graphics_support_requested)
        {
            return WindowSystem{ .graphics_system = &*graphics_system_integration };
        }
        return WindowSystem{};
    }
};

auto window_plugin_injection(
    const kiln::util::OptionalRef<GraphicsSystemIntegrationPlugin>
        graphics_system_integration_plugin
) -> WindowPlugin
{
    return WindowPlugin{
        .supports_graphics = graphics_system_integration_plugin.has_value(),
    };
}

struct RenderSystem {
    GraphicsSystemIntegration& graphics_system;
    WindowSystem*              window_system{};
};

struct RendererPlugin : kiln::app::PluginInterface {
    bool headless = true;

    auto build(
        GraphicsSystemIntegration&                  graphics_system_integration,
        const kiln::util::OptionalRef<WindowSystem> window_system
    ) const -> RenderSystem
    {
        return RenderSystem{
            .graphics_system = graphics_system_integration,
            .window_system   = headless ? nullptr : &*window_system,
        };
    }
};

struct RendererPluginInjection {
    bool presentation_support_requested = true;

    auto operator()(
        const GraphicsSystemIntegrationPlugin&,
        const kiln::util::OptionalRef<WindowPlugin> window_plugin
    ) const -> RendererPlugin
    {
        if (presentation_support_requested)
        {
            PRECOND(window_plugin.has_value());
        }

        if (window_plugin.has_value() && window_plugin->supports_graphics)
        {
            window_plugin->graphics_support_requested = true;
            return RendererPlugin{ .headless = false };
        }

        return RendererPlugin{ .headless = true };
    }
};

auto main() -> int
{
    using namespace kiln;

    app::App app = app::create()
                       .insert_plugin(GraphicsSystemIntegrationPlugin{})
                       .inject_plugin(RendererPluginInjection{})
                       .inject_plugin(window_plugin_injection)
                       .build();

    // Renderer is never headless when both window and graphics plugins are present
    std::println(
        "{}",
        app.context().at<RenderSystem>().window_system == nullptr
            ? "Renderer is headless"
            : "Renderer is not headless"
    );
}
