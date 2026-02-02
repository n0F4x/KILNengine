export module kiln.util.type_traits.type_list_to;

namespace kiln::util {

namespace internal {

template <typename From_T, template <typename...> typename To_T>
struct TypeListTo;

template <
    template <typename...> typename From_T,
    typename... Ts,
    template <typename...> typename To_T>
struct TypeListTo<From_T<Ts...>, To_T> {
    using type = To_T<Ts...>;
};

}   // namespace internal

export template <typename From_T, template <typename...> typename To_T>
using type_list_to_t = typename internal::TypeListTo<From_T, To_T>::type;

}   // namespace kiln::util
