#pragma once

#include <algorithm>
#include <cstdint>
#include <format>
#include <type_traits>
#include <utility>
#include <vector>

#include "kiln/util/concepts/naked.hpp"
#include "kiln/util/concepts/storable.hpp"
#include "kiln/util/concepts/type_list_all_of.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/Function.hpp"
#include "kiln/util/GenericStack.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits/arguments_of.hpp"
#include "kiln/util/type_traits/forward_like.hpp"
#include "kiln/util/type_traits/result_of.hpp"

namespace kiln::util {

namespace internal {

template <typename Any_T>
struct IsBasicGenericStackItemDependency {
    template <typename T>
    using type =
        std::bool_constant<basic_generic_stack_item_c<std::remove_cvref_t<T>, Any_T>>;
};

}   // namespace internal

template <typename T, typename Any_T>
concept basic_generic_stack_item_injection_c =
    naked_c<T> && storable_c<T> && generic_stack_item_c<result_of_t<T&&>>
    && type_list_all_of_c<
        arguments_of_t<T&&>,
        internal::IsBasicGenericStackItemDependency<Any_T>::template type>;

template <typename T, typename Any_T>
concept decays_to_basic_generic_stack_item_injection_c =
    basic_generic_stack_item_injection_c<std::decay_t<T>, Any_T>;

template <move_only_any_c Any_T = BasicMoveOnlyAny<0>>
    requires(Any_T::size == 0)
class BasicGenericStackBuilder {
public:
    using Any = Any_T;

    template <decays_to_basic_generic_stack_item_injection_c<Any_T>... Injections_T>
    explicit BasicGenericStackBuilder(Injections_T&&... injections);

    template <decays_to_basic_generic_stack_item_injection_c<Any_T> Injection_T>
    auto inject(Injection_T&& injection) -> std::decay_t<Injection_T>&;

    template <basic_generic_stack_item_injection_c<Any_T> Injection_T, typename Self_T>
    [[nodiscard]]
    auto injection(this Self_T&&) -> forward_like_t<Injection_T, Self_T>;

    [[nodiscard]]
    auto build() && -> BasicGenericStack<Any_T>;

private:
    using ErasedInjection = MoveOnlyFunction<void(GenericStack&) &&, 0>;

    template <typename Injection_T, typename Item_T>
    struct WrappedInjection {
        Injection_T injection;

        auto operator()(GenericStack& generic_stack) && -> void;
    };

    BasicGenericStack<ErasedInjection> m_injections;
    std::vector<uint64_t>              m_item_types;

    template <generic_stack_item_c Item_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;
};

using GenericStackBuilder = BasicGenericStackBuilder<>;

template <typename T>
concept generic_stack_item_injection_c =
    basic_generic_stack_item_injection_c<T, GenericStackBuilder::Any>;

template <typename T>
concept decays_to_generic_stack_item_injection_c =
    generic_stack_item_injection_c<std::decay_t<T>>;

}   // namespace kiln::util

namespace kiln::util {

namespace internal {

template <typename Injection_T>
auto apply_injection(Injection_T&& injection, GenericStack& generic_stack)
    -> result_of_t<Injection_T>
{
    using Parameters = arguments_of_t<Injection_T>;
    static_assert(type_list_all_of_c<Parameters, std::is_lvalue_reference>);

    return [&injection, &generic_stack]<typename... Parameters_T>(
               TypeList<Parameters_T...>
           ) -> result_of_t<Injection_T>   //
    {
        (PRECOND(
             (generic_stack.contains<std::remove_cvref_t<Parameters_T>>()),
             std::format(
                 "Missing dependency `{}` for injection `{}`",
                 name_of<std::remove_cvref_t<Parameters_T>>(),
                 name_of<Injection_T>()
             )
         ),
         ...);

        return std::invoke(
            std::forward<Injection_T>(injection),   //
            [&generic_stack]<typename Parameter_T> -> decltype(auto) {
                return generic_stack.at<std::remove_cvref_t<Parameter_T>>();
            }.template operator()<Parameters_T>()...
        );
    }(Parameters{});
}

}   // namespace internal

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <decays_to_basic_generic_stack_item_injection_c<Any_T>... Injections_T>
BasicGenericStackBuilder<Any_T>::BasicGenericStackBuilder(Injections_T&&... injections)
{
    (inject(std::forward<Injections_T>(injections)), ...);
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <decays_to_basic_generic_stack_item_injection_c<Any_T> Injection_T>
auto BasicGenericStackBuilder<Any_T>::inject(Injection_T&& injection)
    -> std::decay_t<Injection_T>&
{
    using Item             = std::remove_cvref_t<result_of_t<Injection_T>>;
    using DecayedInjection = std::decay_t<Injection_T>;
    using WrappedInjection = WrappedInjection<DecayedInjection, Item>;

    PRECOND(
        (!contains<Item>()),
        std::format(
            "Attempt to inject type `{}`, but it has already been injected",
            name_of<Item>()
        )
    );

    WrappedInjection& wrapped_injection = m_injections.emplace<WrappedInjection>(
        WrappedInjection{ std::forward<Injection_T>(injection) }
    );

    m_item_types.push_back(hash_u64<Item>());

    return wrapped_injection.injection;
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <basic_generic_stack_item_injection_c<Any_T> Injection_T, typename Self_T>
auto BasicGenericStackBuilder<Any_T>::injection(this Self_T&& self)
    -> forward_like_t<Injection_T, Self_T>
{
    return std::forward_like<Self_T>(self.m_injections)
        .template at<WrappedInjection<Injection_T, result_of_t<Injection_T>>>()
        .injection;
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <typename Injection_T, typename Item_T>
auto BasicGenericStackBuilder<Any_T>::WrappedInjection<Injection_T, Item_T>::operator()(
    GenericStack& generic_stack
) && -> void
{
    generic_stack.emplace<Item_T>(
        internal::apply_injection(std::move(injection), generic_stack)
    );
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
auto BasicGenericStackBuilder<Any_T>::build() && -> BasicGenericStack<Any_T>
{
    BasicGenericStack<Any_T> result;

    std::move(m_injections).for_each([&result](ErasedInjection&& injection) -> void {
        std::move(injection)(result);
    });

    return result;
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <generic_stack_item_c Item_T>
auto BasicGenericStackBuilder<Any_T>::contains() const noexcept -> bool
{
    return std::ranges::contains(m_item_types, hash_u64<Item_T>());
}

}   // namespace kiln::util
