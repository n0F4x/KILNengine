#pragma once

#include "kiln/util/concepts/nothrow_constructible_from.hpp"
#include "kiln/util/concepts/nothrow_convertible_to.hpp"

namespace kiln::util {

template <typename T>
concept nothrow_move_constructible_c = nothrow_constructible_from_c<T, T>
                                    && nothrow_convertible_to_c<T, T>;

}   // namespace kiln::util
