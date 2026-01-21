#pragma once

namespace kiln::util {

namespace internal {

template <typename TypeList_T>
struct TypeListDropFront;

template <template <typename...> typename TypeList_T, typename T, typename... Ts>
struct TypeListDropFront<TypeList_T<T, Ts...>> {
    using type = TypeList_T<Ts...>;
};

}   // namespace internal

template <typename TypeList_T>
using type_list_drop_front_t = typename internal::TypeListDropFront<TypeList_T>::type;

}   // namespace kiln::util
