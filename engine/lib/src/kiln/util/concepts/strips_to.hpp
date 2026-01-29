#pragma once

#include <concepts>
#include <type_traits>

namespace kiln::util {

template <typename T, typename U>
concept strips_to_c = std::same_as<std::remove_cvref_t<T>, U>;

}   // namespace kiln::util
