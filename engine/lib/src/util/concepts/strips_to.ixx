module;

#include <concepts>
#include <type_traits>

export module kiln.util.concepts.strips_to;

namespace kiln::util {

export template <typename T, typename U>
concept strips_to_c = std::same_as<std::remove_cvref_t<T>, U>;

}   // namespace kiln::util
