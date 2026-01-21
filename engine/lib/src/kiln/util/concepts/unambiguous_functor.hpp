#pragma once

#include "kiln/util/concepts/unambiguous_explicit_functor.hpp"
#include "kiln/util/concepts/unambiguous_implicit_functor.hpp"
#include "kiln/util/concepts/unambiguous_static_functor.hpp"

namespace kiln::util {

template <typename T>
concept unambiguous_functor_c = unambiguous_explicit_functor_c<T>
                             || unambiguous_implicit_functor_c<T>
                             || unambiguous_static_functor_c<T>;

}   // namespace kiln::util
