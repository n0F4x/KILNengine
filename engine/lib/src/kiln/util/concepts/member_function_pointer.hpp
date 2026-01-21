#pragma once

#include <type_traits>

namespace kiln::util {

template <typename F>
concept member_function_pointer_c = std::is_member_function_pointer_v<F>;

}   // namespace kiln::util
