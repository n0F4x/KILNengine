module;

#include <concepts>
#include <type_traits>

export module kiln.app.plugin.plugin_c;

import kiln.app.context.context_variable_c;
import kiln.app.plugin.PluginInterface;
import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;
import kiln.util.type_traits.Signature;

namespace kiln::app {

template <typename T>
struct IsPotentiallyOptionalContextVariableRef {
    constexpr static bool value =
        // TODO: remove double requires with better compiler
        requires {
            requires(std::is_lvalue_reference_v<T>
                     && context_variable_c<std::remove_cvref_t<T>>)
                        || (util::specialization_of_c<T, util::OptionalRef>
                            && context_variable_c<std::remove_cvref_t<
                                typename std::remove_reference_t<T>::ValueType>>);
        };
};

export template <typename T>
concept plugin_c =
    util::naked_c<T>                           //
    && std::derived_from<T, PluginInterface>   //
    && requires {
           requires context_variable_c<util::result_of_t<decltype(&T::operator())>>
                        && util::type_list_all_of_c<
                            util::arguments_of_t<decltype(&T::operator())>,
                            IsPotentiallyOptionalContextVariableRef>;
       };

export template <typename T>
concept decays_to_plugin_c = plugin_c<std::remove_cvref_t<T>>;

}   // namespace kiln::app
