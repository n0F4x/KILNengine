module;

#include <concepts>
#include <type_traits>

export module kiln.reg.represents_entry_builder_dependency_c;

import kiln.reg.EntryBuilderBase;
import kiln.reg.EntryTraits;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;

namespace kiln::reg {

export template <typename T>
concept represents_entry_builder_dependency_c
    = (std::is_lvalue_reference_v<T>
       && (std::is_base_of_v<internal::EntryBuilderBase, std::remove_reference_t<T>>
           || requires { requires EntryTraits<T>::is_configuration_entry; }))
   || (util::specialization_of_c<T, util::OptionalRef>
       && (std::is_base_of_v<
               internal::EntryBuilderBase,
               std::remove_reference_t<typename T::ValueType>>
           || requires { requires EntryTraits<T>::is_configuration_entry; }));

}   // namespace kiln::reg
