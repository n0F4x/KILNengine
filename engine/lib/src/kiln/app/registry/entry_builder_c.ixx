module;

#include <concepts>
#include <type_traits>

export module kiln.app.registry.entry_builder_c;

import kiln.app.registry.entry_c;
import kiln.app.registry.EntryBuilderInterface;
import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.concepts.type_list_all_of;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.result_of;

namespace kiln::app {

template <typename T>
struct RepresentsEntryDependency {
    constexpr static bool value = requires {
        requires(std::is_lvalue_reference_v<T> && entry_c<std::remove_cvref_t<T>>)
                    || (util::specialization_of_c<T, util::OptionalRef>
                        && entry_c<std::remove_cvref_t<typename T::ValueType>>);
    };   // namespace kiln::app
};

export template <typename T>
concept entry_builder_c
    = util::naked_c<T>                                //
   && std::derived_from<T, EntryBuilderInterface>   //
   && requires {
          requires entry_c<util::result_of_t<decltype(&T::build)>>
                       && util::type_list_all_of_c<
                           util::arguments_of_t<decltype(&T::build)>,
                           RepresentsEntryDependency>;
      };

export template <typename T>
concept decays_to_entry_builder_c = entry_builder_c<std::remove_cvref_t<T>>;

}   // namespace kiln::app
