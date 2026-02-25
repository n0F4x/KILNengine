module;

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include "no_unique_address.hpp"

export module kiln.util.Function;

import kiln.util.Any;
import kiln.util.concepts.function;
import kiln.util.concepts.specialization_of;
import kiln.util.type_traits.arguments_of;
import kiln.util.type_traits.is_noexcept;
import kiln.util.type_traits.result_of;
import kiln.util.type_traits.Signature;

namespace kiln::util {

namespace internal {

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    typename ArgumentList_T>
struct FunctionTraits;

}   // namespace internal

export template <
    function_c  Signature_T,
    bool        is_move_only_T = false,
    std::size_t size_T         = 3 * sizeof(void*),
    std::size_t alignment_T    = sizeof(void*)>
using Function = BasicAny<typename internal::FunctionTraits<
    is_move_only_T,
    size_T,
    alignment_T,
    Signature_T,
    arguments_of_t<Signature_T>>::AnyTraits>;

export template <
    function_c  Signature_T,
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using CopyableFunction = Function<Signature_T, false, size_T, alignment_T>;

export template <
    function_c  Signature_T,
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using MoveOnlyFunction = Function<Signature_T, true, size_T, alignment_T>;

}   // namespace kiln::util

namespace kiln::util::internal {

template <typename T>
using ensure_reference_t = std::conditional_t<std::is_reference_v<T>, T, T&>;

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    typename Signature_T,
    template <typename...> typename ArgumentList_T,
    typename... FArgs_T>
struct FunctionTraits<
    is_move_only_T,
    size_T,
    alignment_T,
    Signature_T,
    ArgumentList_T<FArgs_T...>>   //
{
    using Result = result_of_t<Signature_T>;

    template <typename Any_T>
    using invoke_qualified_self_t =
        ensure_reference_t<typename Signature<Signature_T>::template mimic_t<Any_T>>;

    consteval static auto is_noexcept() noexcept -> bool
    {
        return is_noexcept_v<Signature_T>;
    }

    template <typename T>
    consteval static auto mimics_qualifiers() noexcept -> bool
    {
        return Signature<Signature_T>::template mimics_qualifiers<T>;
    }

    template <typename Any_T>
    struct VTable {
        using CallFunc = auto(invoke_qualified_self_t<Any_T>, FArgs_T...) noexcept(
            is_noexcept()
        ) -> Result;

        std::reference_wrapper<CallFunc> call;

        template <typename F>
        struct Operations {
            // ReSharper disable once CppNotAllPathsReturnValue
            constexpr static auto call(
                invoke_qualified_self_t<Any_T> that,
                FArgs_T... args
            ) noexcept(is_noexcept()) -> Result
            {
                if constexpr (std::is_void_v<Result>)
                {
                    std::invoke(
                        any_cast<std::decay_t<F>>(
                            std::forward<invoke_qualified_self_t<Any_T>>(that)
                        ),
                        std::forward<FArgs_T>(args)...
                    );
                }
                else
                {
                    return std::invoke(
                        any_cast<std::decay_t<F>>(
                            std::forward<invoke_qualified_self_t<Any_T>>(that)
                        ),
                        std::forward<FArgs_T>(args)...
                    );
                }
            }

            constexpr static VTable vtable{
                .call = call,
            };
        };
    };

    template <typename Any_T>
    class Interface {
    public:
        constexpr explicit Interface(const AnyExtraVTableAccessor& extra_vtable_accessor)
            : m_extra_vtable{ extra_vtable_accessor }
        {
        }

        template <typename Self_T>
            requires(mimics_qualifiers<Self_T &&>())
        constexpr auto operator()(this Self_T&& self, FArgs_T... args) noexcept(
            is_noexcept()
        ) -> Result
        {
            return self.Interface::m_extra_vtable(self).call(
                static_cast<invoke_qualified_self_t<Any_T>>(self),
                std::forward<FArgs_T>(args)...
            );
        }

    private:
        [[kiln_no_unique_address]]
        AnyExtraVTableAccessor m_extra_vtable;
    };

    template <typename F>
    struct Policy {
        using Mimicked = Signature<Signature_T>::template mimic_t<std::decay_t<F>>;

        constexpr static bool value =
            is_noexcept_v<Signature_T>
                ? std::is_nothrow_invocable_r_v<Result, Mimicked, FArgs_T...>
                : std::is_invocable_r_v<Result, Mimicked, FArgs_T...>;
    };

    using AnyTraits =
        DefaultAnyTraits<is_move_only_T, size_T, alignment_T, Policy, Interface, VTable>;
};

}   // namespace kiln::util::internal
