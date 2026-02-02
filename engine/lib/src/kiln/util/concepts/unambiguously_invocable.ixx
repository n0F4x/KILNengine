module;

#include <type_traits>

export module kiln.util.concepts.unambiguously_invocable;

import kiln.util.concepts.function;
import kiln.util.concepts.function_pointer;
import kiln.util.concepts.function_reference;
import kiln.util.concepts.member_function_pointer;
import kiln.util.concepts.unambiguous_functor;

namespace kiln::util {

export template <typename T>
concept unambiguously_invocable_c = function_c<T>                                    //
                                 || function_pointer_c<std::remove_reference_t<T>>   //
                                 || function_reference_c<T>
                                 || member_function_pointer_c<T>
                                 || unambiguous_functor_c<T>;

}   // namespace kiln::util
