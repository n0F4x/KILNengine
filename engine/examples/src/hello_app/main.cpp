#include <cstdio>

import kiln.app;
import kiln.util.containers.OptionalRef;

struct GraphicsSystemIntegration : kiln::app::EntryBase {};

template <typename Entry_T>
struct BuildDescriber {
    constexpr static auto operator()(
        kiln::app::EntryBuildDirector<Entry_T>& build_director
    ) -> void;
};

struct WindowSystem
    : kiln::app::BuildableEntry<WindowSystem, BuildDescriber<WindowSystem>{}> {
    struct Builder;

    GraphicsSystemIntegration* graphics_system{};
};

struct WindowSystem::Builder : kiln::app::EntryBuilderBase {
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

struct RenderSystem
    : kiln::app::BuildableEntry<RenderSystem, BuildDescriber<RenderSystem>{}>   //
{
    struct Builder;

    GraphicsSystemIntegration& graphics_system;
    WindowSystem*              window_system{};
};

struct RenderSystem::Builder : kiln::app::EntryBuilderBase {
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

template <typename Entry_T>
constexpr auto BuildDescriber<Entry_T>::operator()(
    kiln::app::EntryBuildDirector<Entry_T>& build_director
) -> void
{
    build_director.template use_builder<typename Entry_T::Builder>();
}

auto main() -> int
{
    /*
     * GraphicsSystemIntegration gets implicitly injected
     *  as RenderSystem unconditionally depends on it.
     * WindowSystem gets built before RenderSystem
     *  as RenderSystem (conditionally) depends on it.
     */
    kiln::app::App app = kiln::app::create()
                             .register_entry<RenderSystem>()
                             .register_entry<WindowSystem>()
                             .build();

    /*
     * RenderSystem is never headless when the WindowSystem is present
     */
    std::puts(
        app.registry().at<RenderSystem>().window_system == nullptr
            ? "Renderer is headless"
            : "Renderer is not headless"
    );
}
