module;

#include <concepts>
#include <type_traits>

export module kiln.util.concepts.naked;

namespace kiln::util {

export template <typename T>
concept naked_c = std::same_as<T, std::remove_cvref_t<T>>;

}   // namespace kiln::util
