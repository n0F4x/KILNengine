#pragma once

#include <type_traits>

namespace kiln::util {

namespace internal {

template <typename, template <typename...> typename>
struct is_specialization_of : std::false_type {};

template <template <typename...> typename TypeList_T, typename... Ts>
struct is_specialization_of<TypeList_T<Ts...>, TypeList_T> : std::true_type {};

}   // namespace internal

template <typename T, template <typename...> typename TypeList_T>
concept specialization_of_c = internal::is_specialization_of<T, TypeList_T>::value;

}   // namespace kiln::util
