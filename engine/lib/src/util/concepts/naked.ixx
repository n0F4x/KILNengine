module;

#include <type_traits>

export module kiln.util.concepts.naked;

import kiln.util.concepts.strips_to;

namespace kiln::util {

export template <typename T>
concept naked_c = strips_to_c<T, std::remove_cvref_t<T>>;

}   // namespace kiln::util
