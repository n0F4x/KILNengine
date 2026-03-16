module;

#include <functional>

export module kiln.app.plugin.ErasedPlugin;

import kiln.app.App;
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.util.Any;
import kiln.util.concepts.specialization_of;
import kiln.util.OptionalRef;
import kiln.util.type_traits.arguments_of;
import kiln.util.TypeList;

namespace kiln::app {

template <typename T>
struct ErasedPluginPolicy {
    constexpr static bool value{ plugin_c<T> || meta_plugin_c<T> };
};

template <typename ErasedPlugin_T>
class ErasedPluginInterfaceMixin {
public:
    explicit ErasedPluginInterfaceMixin(
        const util::AnyExtraVTableAccessor extra_vtable_accessor
    )
        : m_extra_vtable{ extra_vtable_accessor }
    {
    }

    auto configure_and_build(App& app) && -> void
    {
        m_extra_vtable(static_cast<ErasedPlugin_T&>(*this))
            .configure_and_build(std::move(static_cast<ErasedPlugin_T&>(*this)), app);
    }

private:
    util::AnyExtraVTableAccessor m_extra_vtable;
};

template <typename ErasedPlugin_T>
struct ErasedPluginExtraVTable {
    using BuildFunc = auto(ErasedPlugin_T&&, App& app) -> void;

    std::reference_wrapper<BuildFunc> configure_and_build;

    template <typename Plugin_T>
    struct Operations {
        template <util::specialization_of_c<util::OptionalRef>
                      PotentiallyOptionalContextVariableRef_T>
        static auto fetch_dependency(App& app) -> PotentiallyOptionalContextVariableRef_T
        {
            return app.context()
                .find<std::remove_cvref_t<
                    typename PotentiallyOptionalContextVariableRef_T::ValueType>>();
        }

        template <typename PotentiallyOptionalContextVariableRef_T>
            requires(std::is_lvalue_reference_v<PotentiallyOptionalContextVariableRef_T>)
        static auto fetch_dependency(App& app) -> PotentiallyOptionalContextVariableRef_T
        {
            return app.context()
                .at<std::remove_cvref_t<PotentiallyOptionalContextVariableRef_T>>();
        }

        static auto build(Plugin_T&& plugin, App& app) -> void
        {
            [&plugin, &app]<typename... PotentiallyOptionalContextVariableRefs_T>(
                util::TypeList<PotentiallyOptionalContextVariableRefs_T...>
            ) -> void
            {
                app.context().insert(
                    std::move(plugin).build(
                        fetch_dependency<PotentiallyOptionalContextVariableRefs_T>(app)...
                    )
                );
            }(util::arguments_of_t<decltype(&Plugin_T::build)>{});
        }

        static auto configure_and_build(ErasedPlugin_T&& erased_plugin, App& app) -> void
        {
            Plugin_T&& plugin{ util::any_cast<Plugin_T>(std::move(erased_plugin)) };

            plugin = std::move(plugin).configure(app);

            if constexpr (!meta_plugin_c<Plugin_T>)
            {
                build(std::move(plugin), app);
            }
        }

        constexpr static ErasedPluginExtraVTable vtable{
            .configure_and_build = configure_and_build,
        };
    };
};

export using ErasedPlugin = util::BasicAny<util::DefaultAnyTraits<
    true,
    0,
    0,
    ErasedPluginPolicy,
    ErasedPluginInterfaceMixin,
    ErasedPluginExtraVTable>>;

}   // namespace kiln::app
