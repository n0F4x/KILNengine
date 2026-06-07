module;

#include <type_traits>

export module kiln.util.concepts.is_noexcept;

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
struct IsNoexcept;

template <function_c F>
struct IsNoexcept<F> {
    constexpr static bool value = Signature<F>::is_noexcept();
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct IsNoexcept<F> {
    constexpr static bool value
        = Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::is_noexcept();
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
    constexpr static bool value
        = IsNoexcept<decltype(&std::remove_cvref_t<F>::operator())>::value;
};

}   // namespace internal

export template <typename F>
concept is_noexcept_c = internal::IsNoexcept<F>::value;

}   // namespace kiln::util
