module;

#include <concepts>
#include <type_traits>

export module kiln.app.context.context_builder_c;

import kiln.app.context.context_c;
import kiln.app.context.ContextBuilderInterface;
import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;

namespace kiln::app {

template <typename T>
struct RepresentsContextDependency {
    constexpr static bool value = requires {
        requires(std::is_lvalue_reference_v<T> && context_c<std::remove_cvref_t<T>>)
                    || (util::specialization_of_c<T, util::OptionalRef>
                        && context_c<std::remove_cvref_t<typename T::ValueType>>);
    };   // namespace kiln::app
};

export template <typename T>
concept context_builder_c = util::naked_c<T>                                //
                         && std::derived_from<T, ContextBuilderInterface>   //
                         && requires {
                                requires context_c<util::result_of_t<decltype(&T::build)>>
                                             && util::type_list_all_of_c<
                                                 util::arguments_of_t<decltype(&T::build)>,
                                                 RepresentsContextDependency>;
                            };

export template <typename T>
concept decays_to_concept_builder_c = context_builder_c<std::remove_cvref_t<T>>;

}   // namespace kiln::app
