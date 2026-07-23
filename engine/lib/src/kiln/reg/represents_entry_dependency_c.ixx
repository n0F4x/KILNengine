module;

#include <concepts>
#include <type_traits>

export module kiln.reg.represents_entry_dependency_c;

import kiln.reg.entry_c;
import kiln.reg.EntryBuilderBase;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.OptionalRef;

namespace kiln::reg {

export template <typename T>
concept represents_entry_dependency_c
    = (std::is_lvalue_reference_v<T>
       && (entry_c<std::remove_cvref_t<T>>
           || (std::is_base_of_v<internal::EntryBuilderBase, std::remove_cvref_t<T>>
               && std::is_const_v<std::remove_reference_t<T>>)))
   || (util::specialization_of_c<T, util::OptionalRef>
       && (entry_c<std::remove_cvref_t<typename T::ValueType>>
           || (std::is_base_of_v<
                   internal::EntryBuilderBase,
                   std::remove_cvref_t<typename T::ValueType>>
               && std::is_const_v<std::remove_reference_t<typename T::ValueType>>)));

}   // namespace kiln::reg
