#pragma once

#include <type_traits>

#include "kiln/util/concepts/function.hpp"
#include "kiln/util/concepts/function_pointer.hpp"
#include "kiln/util/concepts/function_reference.hpp"
#include "kiln/util/concepts/member_function_pointer.hpp"
#include "kiln/util/concepts/unambiguous_functor.hpp"
#include "kiln/util/concepts/unambiguously_invocable.hpp"
#include "kiln/util/type_traits/Signature.hpp"

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

template <typename F>
using result_of_t = typename internal::ResultOf<F>::type;

}   // namespace kiln::util
