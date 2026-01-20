module;

#include <type_traits>

export module kiln.util.concepts.specialization_of;

namespace kiln::util {

template <typename, template <typename...> typename>
struct is_specialization_of : std::false_type {};

template <template <typename...> typename TypeList_T, typename... Ts>
struct is_specialization_of<TypeList_T<Ts...>, TypeList_T> : std::true_type {};

export template <typename T, template <typename...> typename TypeList_T>
concept specialization_of_c = is_specialization_of<T, TypeList_T>::value;

}   // namespace kiln::util
