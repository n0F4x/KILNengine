#pragma once

#include <type_traits>

#include "kiln/util/concepts/function.hpp"
#include "kiln/util/concepts/function_pointer.hpp"
#include "kiln/util/concepts/function_reference.hpp"
#include "kiln/util/concepts/member_function_pointer.hpp"
#include "kiln/util/concepts/unambiguous_functor.hpp"

namespace kiln::util {

template <typename T>
concept unambiguously_invocable_c = function_c<T>                                    //
                                 || function_pointer_c<std::remove_reference_t<T>>   //
                                 || function_reference_c<T>
                                 || member_function_pointer_c<T>
                                 || unambiguous_functor_c<T>;

}   // namespace kiln::util
