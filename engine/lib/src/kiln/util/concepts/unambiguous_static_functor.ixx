module;

#include <type_traits>

export module kiln.util.concepts.unambiguous_static_functor;

import kiln.util.concepts.unambiguous_explicit_functor;

namespace kiln::util {

export template <typename T>
concept unambiguous_static_functor_c
    = std::is_class_v<std::remove_cvref_t<T>>   //
   && requires { &std::remove_cvref_t<T>::operator(); }
   && !std::is_member_function_pointer_v<decltype(&std::remove_cvref_t<T>::operator())>
   && !unambiguous_explicit_functor_c<T>;

}   // namespace kiln::util
