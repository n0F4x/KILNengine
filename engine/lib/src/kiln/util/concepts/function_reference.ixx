module;

#include <type_traits>

export module kiln.util.concepts.function_reference;

namespace kiln::util {

export template <typename T>
concept function_reference_c
    = std::is_reference_v<T> && std::is_function_v<std::remove_reference_t<T>>;

}   // namespace kiln::util
