module;

#include <concepts>
#include <type_traits>

export module kiln.app.registry.represents_entry_builder_dependency_c;

import kiln.app.registry.configuration_entry_c;
import kiln.app.registry.EntryBuilderBase;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;

namespace kiln::app {

export template <typename T>
concept represents_entry_builder_dependency_c
    = (std::is_lvalue_reference_v<T>
       && (std::derived_from<std::remove_cvref_t<T>, EntryBuilderBase>
           || configuration_entry_c<std::remove_cvref_t<T>>))
   || (util::specialization_of_c<T, util::OptionalRef>
       && (std::derived_from<std::remove_cvref_t<typename T::ValueType>, EntryBuilderBase>
           || configuration_entry_c<std::remove_cvref_t<typename T::ValueType>>));

}   // namespace kiln::app
