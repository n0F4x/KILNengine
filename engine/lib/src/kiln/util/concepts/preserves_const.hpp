#pragma once

#include <type_traits>

namespace kiln::util {

template <typename To_T, typename From_T>
concept preserves_const_c = !(std::is_const_v<From_T> && not std::is_const_v<To_T>);

}   // namespace kiln::util
