module;

#include <concepts>
#include <functional>
#include <span>
#include <vector>

export module kiln.app.plugin.PluginInterface;

import kiln.app.App;
import kiln.app.context.context_variable_c;
import kiln.util.Function;
import kiln.util.concepts.naked;
import kiln.util.concepts.type_list_all_of;
import kiln.util.reflection;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.type_list_drop_front;
import kiln.util.type_traits.type_list_front;
import kiln.util.TypeList;

namespace kiln::app {

template <typename T>
struct IsContextVariable {
    constexpr static bool value{ context_variable_c<T> };
};

template <typename T, typename Plugin_T>
concept configures_c = util::naked_c<Plugin_T> && requires {
    requires std::same_as<util::type_list_front_t<util::arguments_of_t<T>>, Plugin_T&>;
    requires util::type_list_all_of_c<
        util::type_list_drop_front_t<util::arguments_of_t<T>>,
        IsContextVariable>;
};

export class PluginInterface {
    using Configurator = util::MoveOnlyFunction<auto(PluginInterface&, App&) &&->void>;

public:
    [[nodiscard]]
    auto configuration_dependency_hashes() const noexcept -> std::span<const uint64_t>;

    template <util::naked_c Self_T, configures_c<Self_T> Configurator_T>
    auto register_configuration(this Self_T& self, Configurator_T&& configurator) -> void;

    template <typename Self_T>
    auto configure(this Self_T&&, App& app) -> Self_T&&;

private:
    std::vector<uint64_t>     m_configuration_dependency_hashes;
    std::vector<Configurator> m_configurators;

    auto insert_configuration_dependency_hash(uint64_t dependency_hash) -> void;
};

}   // namespace kiln::app

namespace kiln::app {

template <util::naked_c Self_T, configures_c<Self_T> Configurator_T>
auto PluginInterface::register_configuration(
    this Self_T&     self,
    Configurator_T&& configurator
) -> void
{
    [&self, &configurator]<typename... ContextVariableRefs_T>(
        util::TypeList<ContextVariableRefs_T...>
    ) -> void
    {
        (self.PluginInterface::insert_configuration_dependency_hash(
             util::hash_u64<std::remove_cvref_t<ContextVariableRefs_T>>()
         ),
         ...);

        self.PluginInterface::m_configurators.emplace_back(
            [x_configurator = std::forward<Configurator_T>(configurator)](
                PluginInterface& future_self, App& app
            ) mutable -> void
            {
                std::invoke(
                    std::move(x_configurator),
                    static_cast<Self_T&>(future_self),
                    app.context().at<std::remove_cvref_t<ContextVariableRefs_T>>()...
                );
            }
        );
    }(util::type_list_drop_front_t<util::arguments_of_t<Configurator_T>>{});
}

template <typename Self_T>
auto PluginInterface::configure(this Self_T&& self, App& app) -> Self_T&&
{
    for (Configurator& configurator : self.PluginInterface::m_configurators)
    {
        std::move(configurator)(self, app);
    }

    return std::move(self);
}

}   // namespace kiln::app
