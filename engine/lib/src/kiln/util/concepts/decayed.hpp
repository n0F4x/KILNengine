#pragma once

#include <concepts>
#include <type_traits>

namespace kiln::util {

template <typename T>
concept decayed_c = std::same_as<T, std::decay_t<T>>;

}   // namespace kiln::util
