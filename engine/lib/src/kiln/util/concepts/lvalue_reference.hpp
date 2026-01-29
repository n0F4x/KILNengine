#pragma once

#include <type_traits>

namespace kiln::util {

template <typename T>
concept lvalue_reference_c = std::is_lvalue_reference_v<T>;

}   // namespace kiln::util
