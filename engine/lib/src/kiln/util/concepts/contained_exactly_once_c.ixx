module;

#include <type_traits>

export module kiln.util.concepts.contained_exactly_once;

namespace kiln::util {

export template <typename T, typename... Ts>
concept contained_exactly_once_c = (int{ std::is_same_v<T, Ts> } + ...) == 1;

}   // namespace kiln::util
