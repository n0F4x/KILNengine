#include <cstdio>

import kiln.app;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;

struct GraphicsSystemIntegration : kiln::app::ContextBase {};

struct WindowSystem : kiln::app::ContextBase {
    struct Builder;

    GraphicsSystemIntegration* graphics_system{};
};

struct WindowSystem::Builder : kiln::app::ContextBuilderInterface {
    bool graphics_support_requested = false;

    auto build(
        const kiln::util::OptionalRef<GraphicsSystemIntegration> graphics_system_integration
    ) const -> WindowSystem
    {
        if (graphics_support_requested && graphics_system_integration.has_value())
        {
            return WindowSystem{ .graphics_system = &*graphics_system_integration };
        }
        return WindowSystem{};
    }
};

struct RenderSystem : kiln::app::ContextBase {
    struct Builder;

    GraphicsSystemIntegration& graphics_system;
    WindowSystem*              window_system{};
};

struct RenderSystem::Builder : kiln::app::ContextBuilderInterface {
    static auto create(
        const kiln::util::OptionalRef<WindowSystem::Builder> window_builder
    ) -> Builder
    {
        if (window_builder.has_value())
        {
            window_builder->graphics_support_requested = true;
        }

        return Builder{};
    }

    static auto build(
        GraphicsSystemIntegration&                  graphics_system_integration,
        const kiln::util::OptionalRef<WindowSystem> window_system
    ) -> RenderSystem
    {
        return RenderSystem{
            .graphics_system = graphics_system_integration,
            .window_system   = window_system.has_value() ? &*window_system : nullptr,
        };
    }
};

auto main() -> int
{
    /*
     * GraphicsSystemIntegration context gets implicitly injected
     *  as the RenderSystem unconditionally depends on it
     */
    kiln::app::App app =      //
        kiln::app::create()   //
            .use_context<RenderSystem>()
            .use_context<WindowSystem>()
            .build();

    /*
     * RenderSystem is never headless when the WindowSystem is present
     */
    std::puts(
        app.contexts().at<RenderSystem>().window_system == nullptr
            ? "Renderer is headless"
            : "Renderer is not headless"
    );
}
