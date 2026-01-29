#pragma once

namespace kiln::util {

namespace internal {

template <typename>
struct TypeListFront;

template <template <typename...> typename TypeList_T, typename T, typename... Ts>
struct TypeListFront<TypeList_T<T, Ts...>> {
    using type = T;
};

}   // namespace internal

template <typename TypeList_T>
using type_list_front_t = typename internal::TypeListFront<TypeList_T>::type;

}   // namespace kiln::util
