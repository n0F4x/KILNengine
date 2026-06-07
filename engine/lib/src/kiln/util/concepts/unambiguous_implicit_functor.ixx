module;

#include <type_traits>

export module kiln.util.concepts.unambiguous_implicit_functor;

namespace kiln::util {

export template <typename T>
concept unambiguous_implicit_functor_c
    = std::is_class_v<std::remove_cvref_t<T>>   //
   && requires { &std::remove_cvref_t<T>::operator(); }
   && std::is_member_function_pointer_v<decltype(&std::remove_cvref_t<T>::operator())>;

}   // namespace kiln::util
