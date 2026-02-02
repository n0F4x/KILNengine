module;

#include <type_traits>

export module kiln.util.type_traits.result_of;

import kiln.util.concepts.function;
import kiln.util.concepts.function_pointer;
import kiln.util.concepts.function_reference;
import kiln.util.concepts.member_function_pointer;
import kiln.util.concepts.unambiguous_functor;
import kiln.util.concepts.unambiguously_invocable;
import kiln.util.type_traits.Signature;

namespace kiln::util {

namespace internal {

template <unambiguously_invocable_c F>
struct ResultOf;

template <function_c F>
struct ResultOf<F> {
    using type = typename Signature<F>::result_t;
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct ResultOf<F> {
    using type =
        typename Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::result_t;
};

template <function_reference_c F>
struct ResultOf<F> {
    using type = typename Signature<std::remove_reference_t<F>>::result_t;
};

template <member_function_pointer_c F>
struct ResultOf<F> {
    using type = typename Signature<F>::result_t;
};

template <unambiguous_functor_c F>
struct ResultOf<F> {
    using type = typename ResultOf<decltype(&std::remove_cvref_t<F>::operator())>::type;
};

}   // namespace internal

export template <typename F>
using result_of_t = typename internal::ResultOf<F>::type;

}   // namespace kiln::util
