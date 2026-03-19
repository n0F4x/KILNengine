module;

#include <concepts>
#include <functional>
#include <span>
#include <vector>

export module kiln.app.plugin.PluginInterface;

import kiln.app.App;
import kiln.app.context.context_variable_c;
import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.containers.OptionalRef;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;
import kiln.util.type_traits.type_list_drop_front;
import kiln.util.type_traits.type_list_front;
import kiln.util.TypeList;

namespace kiln::app {

template <typename Plugin_T>
struct IsContextVariableDependencyRef {
    template <typename T>
    struct type {
        constexpr static bool value{
            std::is_lvalue_reference_v<T> && context_variable_c<std::remove_reference_t<T>>
            && !std::same_as<
                std::remove_cvref_t<T>,
                util::result_of_t<decltype(&Plugin_T::operator())>>   //
        };
    };
};

template <typename T, typename Plugin_T>
concept configures_c = util::naked_c<Plugin_T> && requires {
    requires std::same_as<util::type_list_front_t<util::arguments_of_t<T>>, Plugin_T&>;
    requires util::type_list_all_of_c<
        util::type_list_drop_front_t<util::arguments_of_t<T>>,
        IsContextVariableDependencyRef<Plugin_T>::template type>;
};


export class PluginInterface;

using Configuration = util::MoveOnlyFunction<auto(PluginInterface&, App&) &&->void>;

export [[nodiscard]]
auto configuration_dependency_hash_set(const PluginInterface& plugin) noexcept
    -> std::span<const uint64_t>;

export [[nodiscard]]
auto dependency_hash_set(const PluginInterface& plugin) noexcept
    -> std::span<const uint64_t>;

export auto set_resolved_dependency_hash_set(
    PluginInterface&          plugin,
    std::span<const uint64_t> dependency_hash_set
) -> void;

export template <util::naked_c Plugin_T>
auto build(Plugin_T&&, App& app) -> void;

export class PluginInterface {
public:
    template <util::naked_c Self_T, configures_c<Self_T> Configuration_T>
    auto register_configuration(this Self_T& self, Configuration_T&& configuration)
        -> void;

private:
    friend auto configuration_dependency_hash_set(const PluginInterface& plugin) noexcept
        -> std::span<const uint64_t>;
    friend auto dependency_hash_set(const PluginInterface& plugin) noexcept
        -> std::span<const uint64_t>;

    friend auto set_resolved_dependency_hash_set(
        PluginInterface&          plugin,
        std::span<const uint64_t> dependency_hash_set
    ) -> void;

    template <util::naked_c Self_T>
    friend auto build(Self_T&&, App& app) -> void;


    std::pmr::vector<uint64_t>      m_configuration_dependency_hash_set;
    std::pmr::vector<uint64_t>      m_dependency_hash_set;
    std::pmr::vector<Configuration> m_configurators;


    auto insert_configuration_dependency_hash(uint64_t dependency_hash) -> void;
};

}   // namespace kiln::app

namespace kiln::app {

namespace internal {

template <
    util::specialization_of_c<util::OptionalRef> PotentiallyOptionalContextVariableRef_T>
auto fetch_dependency(App& app) -> PotentiallyOptionalContextVariableRef_T
{
    return app.context()
        .find<std::remove_cvref_t<
            typename PotentiallyOptionalContextVariableRef_T::ValueType>>();
}

template <typename PotentiallyOptionalContextVariableRef_T>
    requires(std::is_lvalue_reference_v<PotentiallyOptionalContextVariableRef_T>)
auto fetch_dependency(App& app) -> PotentiallyOptionalContextVariableRef_T
{
    return app.context().at<std::remove_cvref_t<PotentiallyOptionalContextVariableRef_T>>();
}

template <typename Plugin_T>
auto invoke(Plugin_T&& plugin, App& app) -> void
{
    [&plugin, &app]<typename... PotentiallyOptionalContextVariableRefs_T>(
        util::TypeList<PotentiallyOptionalContextVariableRefs_T...>
    ) -> void
    {
        app.context().insert(
            std::move(plugin)(
                fetch_dependency<PotentiallyOptionalContextVariableRefs_T>(app)...
            )
        );
    }(util::arguments_of_t<decltype(&Plugin_T::operator())>{});
}

}   // namespace internal

template <util::naked_c Plugin_T>
auto build(Plugin_T&& plugin, App& app) -> void
{
    for (Configuration& configurator : plugin.PluginInterface::m_configurators)
    {
        std::move(configurator)(plugin, app);
    }

    if constexpr (requires { &Plugin_T::operator(); })
    {
        internal::invoke(std::move(plugin), app);
    }
}

template <util::naked_c Self_T, configures_c<Self_T> Configuration_T>
auto PluginInterface::register_configuration(
    this Self_T&      self,
    Configuration_T&& configuration
) -> void
{
    [&self, &configuration]<typename... ContextVariableRefs_T>(
        util::TypeList<ContextVariableRefs_T...>
    ) -> void
    {
        (self.PluginInterface::insert_configuration_dependency_hash(
             util::hash_u64<std::remove_cvref_t<ContextVariableRefs_T>>()
         ),
         ...);

        self.PluginInterface::m_configurators.emplace_back(
            [x_configuration = std::forward<Configuration_T>(configuration)](
                PluginInterface& future_self, App& app
            ) mutable -> void
            {
                std::invoke(
                    std::move(x_configuration),
                    static_cast<Self_T&>(future_self),
                    app.context().at<std::remove_cvref_t<ContextVariableRefs_T>>()...
                );
            }
        );
    }(util::type_list_drop_front_t<util::arguments_of_t<Configuration_T>>{});
}

}   // namespace kiln::app
