#include <print>
#include <string_view>

#include <kiln/app.hpp>
#include <kiln/util/OptionalRef.hpp>

struct GraphicsSystemIntegration {};

struct GraphicsSystemIntegrationPlugin {
    static auto operator()(kiln::app::App& app) -> void
    {
        app.resources().insert(GraphicsSystemIntegration{});
    }
};

auto graphics_system_plugin_injection() -> GraphicsSystemIntegrationPlugin
{
    return {};
}

struct WindowSystem {
    GraphicsSystemIntegration* graphics_system{};
};

struct WindowPlugin {
    bool supports_graphics          = false;
    bool graphics_support_requested = false;

    auto operator()(kiln::app::App& app) const -> void
    {
        if (supports_graphics && graphics_support_requested)
        {
            app.resources().inject(
                [](GraphicsSystemIntegration& graphics_system) -> WindowSystem
                {
                    return WindowSystem{
                        .graphics_system = &graphics_system,
                    };
                }
            );
        }
        else
        {
            app.resources().insert(WindowSystem{});
        }
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
    WindowSystem* window_system{};
};

struct RendererPlugin {
    bool headless = true;

    auto operator()(kiln::app::App& app) const -> void
    {
        app.resources().insert(
            RenderSystem{
                .window_system = headless ? nullptr : &app.resources().at<WindowSystem>(),
            }
        );
    }
};

auto renderer_plugin_injection(const kiln::util::OptionalRef<WindowPlugin> window_plugin)
    -> RendererPlugin
{
    if (window_plugin.has_value() && window_plugin->supports_graphics)
    {
        window_plugin->graphics_support_requested = true;
        return RendererPlugin{ .headless = false };
    }

    return RendererPlugin{ .headless = true };
}

auto main() -> int
{
    using namespace kiln;

    using Message = std::string_view;

    app::App app = app::create()
                       .inject_resource(
                           [](const RenderSystem& render_system) -> Message
                           {
                               return render_system.window_system == nullptr
                                        ? "Renderer is headless"
                                        : "Renderer is not headless";
                           }
                       )
                       .plug_in(graphics_system_plugin_injection)
                       .plug_in(renderer_plugin_injection)
                       .plug_in(window_plugin_injection)
                       .build();

    // Renderer is never headless when both window and graphics plugins are present
    std::println("{}", app.resources().at<Message>());
}
