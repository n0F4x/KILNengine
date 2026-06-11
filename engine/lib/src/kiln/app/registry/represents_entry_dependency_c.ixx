module;

#include <type_traits>

export module kiln.app.registry.represents_entry_dependency_c;

import kiln.app.registry.entry_c;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;

namespace kiln::app {

export template <typename T>
concept represents_entry_dependency_c
    = (std::is_lvalue_reference_v<T> && entry_c<std::remove_cvref_t<T>>)
   || (util::specialization_of_c<T, util::OptionalRef>
       && entry_c<std::remove_cvref_t<typename T::ValueType>>);

}   // namespace kiln::app
