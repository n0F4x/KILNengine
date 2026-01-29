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
struct IsNoexcept;

template <function_c F>
struct IsNoexcept<F> {
    constexpr static bool value = Signature<F>::is_noexcept();
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct IsNoexcept<F> {
    constexpr static bool value =
        Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::is_noexcept();
};

template <function_reference_c F>
struct IsNoexcept<F> {
    constexpr static bool value = Signature<std::remove_reference_t<F>>::is_noexcept();
};

template <member_function_pointer_c F>
struct IsNoexcept<F> {
    constexpr static bool value = Signature<F>::is_noexcept();
};

template <unambiguous_functor_c F>
struct IsNoexcept<F> {
    constexpr static bool value =
        IsNoexcept<decltype(&std::remove_cvref_t<F>::operator())>::value;
};

}   // namespace internal

template <typename F>
inline constexpr bool is_noexcept_v = internal::IsNoexcept<F>::value;

}   // namespace kiln::util
