#pragma once

#include <type_traits>

namespace kiln::util {

template <typename T>
concept function_pointer_c = std::is_pointer_v<T>
                          && std::is_function_v<std::remove_pointer_t<T>>;

}   // namespace kiln::util
