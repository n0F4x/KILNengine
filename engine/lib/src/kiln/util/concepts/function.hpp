#pragma once

#include <type_traits>

namespace kiln::util {

template <typename F>
concept function_c = std::is_function_v<F>;

}   // namespace kiln::util
