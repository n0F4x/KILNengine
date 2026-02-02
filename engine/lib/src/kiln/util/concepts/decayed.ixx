module;

#include <concepts>
#include <type_traits>

export module kiln.util.concepts.decayed;

namespace kiln::util {

export template <typename T>
concept decayed_c = std::same_as<T, std::decay_t<T>>;

}   // namespace kiln::util
