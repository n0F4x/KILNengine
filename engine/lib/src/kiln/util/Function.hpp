#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include "kiln/util/Any.hpp"
#include "kiln/util/concepts/function.hpp"
#include "kiln/util/type_traits/arguments_of.hpp"
#include "kiln/util/type_traits/is_noexcept.hpp"
#include "kiln/util/type_traits/result_of.hpp"
#include "kiln/util/type_traits/Signature.hpp"

namespace kiln::util {

namespace internal {

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    typename ArgumentList_T>
class FunctionBase;

}   // namespace internal

template <
    function_c  Signature_T,
    bool        is_move_only_T = false,
    std::size_t size_T         = 3 * sizeof(void*),
    std::size_t alignment_T    = sizeof(void*)>
using Function = internal::
    FunctionBase<is_move_only_T, size_T, alignment_T, Signature_T, arguments_of_t<Signature_T>>;

template <
    function_c  Signature_T,
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using CopyableFunction = Function<Signature_T, false, size_T, alignment_T>;

template <
    function_c  Signature_T,
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using MoveOnlyFunction = Function<Signature_T, true, size_T, alignment_T>;

}   // namespace kiln::util

namespace kiln::util::internal {

template <typename T>
using ensure_reference_t = std::conditional_t<std::is_reference_v<T>, T, T&>;

template <typename FunctionBase, bool is_move_only_T, typename Signature_T, typename... FArgs_T>
struct FunctionBaseTraits {
    using Result = result_of_t<Signature_T>;

    using InvokeQualifiedSelf =
        ensure_reference_t<typename Signature<Signature_T>::template mimic_t<FunctionBase>>;

    consteval static auto is_noexcept() noexcept -> bool
    {
        return is_noexcept_v<Signature_T>;
    }

    struct ExtraVTable {
        using CallFunc = auto (&)(InvokeQualifiedSelf, FArgs_T...) noexcept(is_noexcept())
            -> Result;

        CallFunc call;
    };

    template <typename T>
    consteval static auto mimics_qualifiers() noexcept -> bool
    {
        return Signature<Signature_T>::template mimics_qualifiers<T>;
    }

    template <typename F>
    struct Operations {
        // ReSharper disable once CppNotAllPathsReturnValue
        constexpr static auto call(InvokeQualifiedSelf that, FArgs_T... args) noexcept(
            is_noexcept()
        ) -> Result
        {
            if constexpr (std::is_void_v<Result>)
            {
                std::invoke(
                    any_cast<std::decay_t<F>>(std::forward<InvokeQualifiedSelf>(that)),
                    std::forward<FArgs_T>(args)...
                );
            }
            else
            {
                return std::invoke(
                    any_cast<std::decay_t<F>>(std::forward<InvokeQualifiedSelf>(that)),
                    std::forward<FArgs_T>(args)...
                );
            }
        }

        constexpr static ExtraVTable vtable{
            .call = call,
        };
    };

    template <typename F>
    struct Policy {
        using Mimiced = Signature<Signature_T>::template mimic_t<std::decay_t<F>>;

        constexpr static bool value =
            is_noexcept_v<Signature_T>
                ? std::is_nothrow_invocable_r_v<Result, Mimiced, FArgs_T...>
                : std::is_invocable_r_v<Result, Mimiced, FArgs_T...>;
    };

    using AnyTraits = DefaultAnyTraits<is_move_only_T, ExtraVTable, Operations, Policy>;
};

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> typename ArgList_T,
    typename... FArgs_T>
class FunctionBase<is_move_only_T, size_T, alignment_T, Signature_T, ArgList_T<FArgs_T...>>
    : public BasicAny<
          typename FunctionBaseTraits<
              FunctionBase<is_move_only_T, size_T, alignment_T, Signature_T, ArgList_T<FArgs_T...>>,
              is_move_only_T,
              Signature_T,
              FArgs_T...>::AnyTraits,
          size_T,
          alignment_T>   //
{
    using Traits =
        FunctionBaseTraits<FunctionBase, is_move_only_T, Signature_T, FArgs_T...>;
    using Base                = BasicAny<typename Traits::AnyTraits, size_T, alignment_T>;
    using Result              = Traits::Result;
    using InvokeQualifiedSelf = Traits::InvokeQualifiedSelf;

    consteval static auto is_noexcept() noexcept -> bool
    {
        return Traits::is_noexcept();
    }

public:
    using Base::Base;

    template <typename Self_T>
        requires(Traits::template mimics_qualifiers<Self_T &&>())
    constexpr auto operator()(this Self_T&& self, FArgs_T... args) noexcept(is_noexcept())
        -> Result
    {
        return self
            .template BasicAny<
                typename FunctionBaseTraits<FunctionBase, is_move_only_T, Signature_T, FArgs_T...>::
                    AnyTraits,
                size_T,
                alignment_T>::extra_vtable()
            .call(static_cast<InvokeQualifiedSelf>(self), std::forward<FArgs_T>(args)...);
    }
};

}   // namespace kiln::util::internal
