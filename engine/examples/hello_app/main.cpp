#include <cstdio>

import kiln.app;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;

struct GraphicsSystemIntegration {};

struct GraphicsSystemIntegrationPlugin : kiln::app::PluginInterface {
    static auto create_plugin() -> GraphicsSystemIntegrationPlugin
    {
        return GraphicsSystemIntegrationPlugin{};
    }

    static auto operator()() -> GraphicsSystemIntegration
    {
        return GraphicsSystemIntegration{};
    }
};

struct WindowSystem {
    GraphicsSystemIntegration* graphics_system{};
};

struct WindowPlugin : kiln::app::PluginInterface {
    static auto create_plugin(
        const kiln::util::OptionalRef<GraphicsSystemIntegrationPlugin>
            graphics_system_integration_plugin
    ) -> WindowPlugin
    {
        return WindowPlugin{
            .supports_graphics = graphics_system_integration_plugin.has_value(),
        };
    }

    bool supports_graphics          = false;
    bool graphics_support_requested = false;

    auto operator()(
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

struct RenderSystem {
    GraphicsSystemIntegration& graphics_system;
    WindowSystem*              window_system{};
};

struct RendererPlugin : kiln::app::PluginInterface {
    static auto create_plugin(const kiln::util::OptionalRef<WindowPlugin> window_plugin)
        -> RendererPlugin
    {
        if (window_plugin.has_value() && window_plugin->supports_graphics)
        {
            window_plugin->graphics_support_requested = true;
            return RendererPlugin{ .headless = false };
        }

        return RendererPlugin{ .headless = true };
    }

    bool headless = true;

    auto operator()(
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

auto main() -> int
{
    using namespace kiln;

    app::App app = app::create()
                       .use_plugin<GraphicsSystemIntegrationPlugin>()
                       .use_plugin<RendererPlugin>()
                       .use_plugin<WindowPlugin>()
                       .build();

    // Renderer is never headless when both window and graphics plugins are present
    std::puts(
        app.context().at<RenderSystem>().window_system == nullptr
            ? "Renderer is headless"
            : "Renderer is not headless"
    );
}
