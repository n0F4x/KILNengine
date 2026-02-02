module;

#include <type_traits>

export module kiln.util.concepts.function;

namespace kiln::util {

export template <typename F>
concept function_c = std::is_function_v<F>;

}   // namespace kiln::util
