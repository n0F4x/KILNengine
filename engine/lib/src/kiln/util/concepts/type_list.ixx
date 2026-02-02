module;

#include <type_traits>

export module kiln.util.concepts.type_list;

namespace kiln::util {

namespace internal {

template <typename>
struct IsTypeList : std::false_type {};

template <template <typename...> typename TypeList_T, typename... Ts>
struct IsTypeList<TypeList_T<Ts...>> : std::true_type {};

}   // namespace internal

export template <typename T>
concept type_list_c = internal::IsTypeList<T>::value;

}   // namespace kiln::util
