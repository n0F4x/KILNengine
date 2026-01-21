#pragma once

#include <type_traits>

namespace kiln::util {

template <typename T>
concept function_reference_c = std::is_reference_v<T>
                            && std::is_function_v<std::remove_reference_t<T>>;

}   // namespace kiln::util
