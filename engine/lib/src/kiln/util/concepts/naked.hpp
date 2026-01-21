#pragma once

#include <type_traits>

#include "kiln/util/concepts/strips_to.hpp"

namespace kiln::util {

template <typename T>
concept naked_c = strips_to_c<T, std::remove_cvref_t<T>>;

}   // namespace kiln::util
