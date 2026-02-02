module;

#include <type_traits>

export module kiln.util.concepts.member_function_pointer;

namespace kiln::util {

export template <typename F>
concept member_function_pointer_c = std::is_member_function_pointer_v<F>;

}   // namespace kiln::util
