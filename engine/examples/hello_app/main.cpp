#include <print>

#include <kiln/app.hpp>
#include <kiln/util/OptionalRef.hpp>

struct Window {};

struct WindowPlugin {
    auto operator()(kiln::app::App& app) const -> void
    {
        app.resources().insert(Window{});
    }
};

auto window_plugin_injection() -> WindowPlugin
{
    return WindowPlugin{};
}

struct Renderer {
    Window* window;
};

struct RendererPlugin {
    bool headless;

    auto operator()(kiln::app::App& app) const -> void
    {
        app.resources().insert(
            Renderer{ .window = headless ? nullptr : &app.resources().at<Window>() }
        );
    }
};

auto renderer_plugin_injection(const kiln::util::OptionalRef<WindowPlugin> window_plugin)
    -> RendererPlugin
{
    return RendererPlugin{ .headless = !window_plugin.has_value() };
}

auto main() -> int
{
    using namespace kiln;

    app::App app = app::create()
                       .plug_in(renderer_plugin_injection)
                       .plug_in(window_plugin_injection)
                       .build();

    if (app.resources().at<Renderer>().window == nullptr)
    {
        std::println("Renderer is headless");
    }
    else
    {
        std::println("Renderer is not headless");
    }
}
