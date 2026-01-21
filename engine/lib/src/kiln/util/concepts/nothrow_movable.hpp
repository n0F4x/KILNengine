#pragma once

#include <concepts>

#include "kiln/util/concepts/nothrow_assignable_from.hpp"
#include "kiln/util/concepts/nothrow_move_constructible.hpp"

namespace kiln::util {

template <typename T>
concept nothrow_movable_c = std::is_object_v<T> && nothrow_move_constructible_c<T>
                         && nothrow_assignable_from_c<T&, T> && std::swappable<T>;

}   // namespace kiln::util
