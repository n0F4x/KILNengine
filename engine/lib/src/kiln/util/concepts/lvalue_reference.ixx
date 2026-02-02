module;

#include <type_traits>

export module kiln.util.concepts.lvalue_reference;

namespace kiln::util {

export template <typename T>
concept lvalue_reference_c = std::is_lvalue_reference_v<T>;

}   // namespace kiln::util
