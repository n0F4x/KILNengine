module;

#include <type_traits>

export module kiln.util.type_traits.arguments_of;

import kiln.util.concepts.function;
import kiln.util.concepts.function_pointer;
import kiln.util.concepts.function_reference;
import kiln.util.concepts.member_function_pointer;
import kiln.util.concepts.unambiguous_explicit_functor;
import kiln.util.concepts.unambiguous_implicit_functor;
import kiln.util.concepts.unambiguous_static_functor;
import kiln.util.concepts.unambiguously_invocable;
import kiln.util.type_traits.Signature;
import kiln.util.type_traits.type_list_drop_front;

namespace kiln::util {

namespace internal {

template <unambiguously_invocable_c F>
struct ArgumentsOf;

template <function_c F>
struct ArgumentsOf<F> {
    using type = typename Signature<F>::arguments_t;
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct ArgumentsOf<F> {
    using type =
        typename Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::arguments_t;
};

template <function_reference_c F>
struct ArgumentsOf<F> {
    using type = typename Signature<std::remove_reference_t<F>>::arguments_t;
};

template <member_function_pointer_c F>
struct ArgumentsOf<F> {
    using type = typename Signature<F>::arguments_t;
};

template <unambiguous_explicit_functor_c F>
struct ArgumentsOf<F> {
    using type = type_list_drop_front_t<
        typename ArgumentsOf<decltype(&std::remove_cvref_t<F>::operator())>::type>;
};

template <unambiguous_implicit_functor_c F>
struct ArgumentsOf<F> {
    using type =
        typename ArgumentsOf<decltype(&std::remove_cvref_t<F>::operator())>::type;
};

template <unambiguous_static_functor_c F>
struct ArgumentsOf<F> {
    using type =
        typename ArgumentsOf<decltype(&std::remove_cvref_t<F>::operator())>::type;
};

}   // namespace internal

export template <typename F>
using arguments_of_t = typename internal::ArgumentsOf<F>::type;

}   // namespace kiln::util
