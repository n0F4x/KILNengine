#pragma once

#include <utility>

namespace kiln::util {

template <typename T, typename Like_T>
using forward_like_t = decltype(std::forward_like<Like_T>(std::declval<T>()));

}   // namespace kiln::util
