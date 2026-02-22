#include <any>
#include <print>
#include <string_view>

#include <kiln/util/contract_macros.hpp>

import kiln.app;
import kiln.event;
import kiln.util.contracts;
import kiln.util.OptionalRef;

struct GraphicsSystemIntegration {};

struct GraphicsSystemIntegrationPlugin {
    static auto operator()(kiln::app::App& app) -> void
    {
        app.resources().insert(GraphicsSystemIntegration{});
    }
};

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
    GraphicsSystemIntegration& graphics_system;
    WindowSystem*              window_system{};
};

struct RendererPlugin {
    bool headless = true;

    auto operator()(kiln::app::App& app) const -> void
    {
        app.resources().insert(
            RenderSystem{
                .graphics_system = app.resources().at<GraphicsSystemIntegration>(),
                .window_system = headless ? nullptr : &app.resources().at<WindowSystem>(),
            }
        );
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

struct EventTest {
    int a;
    int b;
};

auto event_test(const int a, const int b) -> void
{
    std::println("Event test, sum:{}", a+b);
}
auto event_test2(const int a, const int b) -> void
{
    std::println("Event test2, diff:{}", a-b);
}

auto main() -> int
{
    using namespace kiln;

    using Message = std::string_view;

    // builder needs to be configured on the stack, otherwise we run into a bug on Windows
    app::Builder builder = app::create()
                                .inject_resource(
                                   [](const RenderSystem& render_system) -> Message
                                   {
                                       return render_system.window_system == nullptr
                                                ? "Renderer is headless"
                                                : "Renderer is not headless";
                                   }
                                )
                                .insert_plugin(GraphicsSystemIntegrationPlugin{})
                                .inject_plugin(RendererPluginInjection{})
                                .inject_plugin(window_plugin_injection)
                                .insert_plugin(event::EventPlugin{});
    app::App app = std::move(builder).build();

    // Renderer is never headless when both window and graphics plugins are present
    std::println("{}", app.resources().at<Message>());

    app.resources().at<event::EventSystem>().subscribe<EventTest>([](const EventTest& ev) -> void {
        event_test(ev.a, ev.b);
    });
    app.resources().at<event::EventSystem>().subscribe<EventTest>([](const EventTest& ev) -> void {
        event_test2(ev.a, ev.b);
    });
    app.resources().at<event::EventSystem>().publish<EventTest>(45, 42);
}
