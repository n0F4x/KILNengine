export module kiln.util.concepts.type_list_all_of;

import kiln.util.concepts.type_list;

namespace kiln::util {

namespace internal {

template <typename, template <typename> typename>
struct TypeListAllOf;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Predicate_T>
struct TypeListAllOf<TypeList_T<Ts...>, Predicate_T> {
    constexpr static bool value{ (Predicate_T<Ts>::value && ...) };
};

}   // namespace internal

export template <typename TypeList_T, template <typename> typename Predicate_T>
concept type_list_all_of_c
    = type_list_c<TypeList_T> && internal::TypeListAllOf<TypeList_T, Predicate_T>::value;

}   // namespace kiln::util
