#pragma once

#include <type_traits>

namespace kiln::util {

template <typename T>
concept storable_c = std::is_nothrow_destructible_v<T>;

}   // namespace kiln::util
