#pragma once

#include <type_traits>

namespace kiln::util {

template <typename To, typename From>
concept preserves_const_c = !(std::is_const_v<From> && not std::is_const_v<To>);

}   // namespace kiln::util
