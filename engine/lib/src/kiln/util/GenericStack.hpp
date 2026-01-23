#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <format>
#include <functional>
#include <vector>

#include "kiln/util/Any.hpp"
#include "kiln/util/concepts/naked.hpp"
#include "kiln/util/concepts/storable.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/OptionalRef.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits/const_like.hpp"
#include "kiln/util/type_traits/forward_like.hpp"

namespace kiln::util {

template <typename T>
concept generic_stack_item_c = storable_c<T> && naked_c<T>;

/*
 * References to contained items are valid until the instance is alive.
 * Destroying a "moved-from" instance does not invalidate any references to items.
 */
template <move_only_any_c Any_T = BasicMoveOnlyAny<0>>
    requires(Any_T::size == 0)
class BasicGenericStack {
public:
    BasicGenericStack()                             = default;
    BasicGenericStack(const BasicGenericStack&)     = delete;
    BasicGenericStack(BasicGenericStack&&) noexcept = default;
    ~BasicGenericStack();

    auto operator=(const BasicGenericStack&) -> BasicGenericStack&     = delete;
    auto operator=(BasicGenericStack&&) noexcept -> BasicGenericStack& = default;

    template <generic_stack_item_c Variable_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Variable_T&;

    template <generic_stack_item_c Variable_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept -> OptionalRef<const_like_t<Variable_T, Self_T>>;

    template <generic_stack_item_c Variable_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> forward_like_t<Variable_T, Self_T>;

    template <generic_stack_item_c Variable_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <typename Self_T, std::invocable<forward_like_t<Any_T, Self_T>> F>
        requires(
            std::is_const_v<std::remove_reference_t<Self_T>>
            || std::is_rvalue_reference_v<Self_T &&>
        )
    auto for_each(this Self_T&&, F&& func) -> F;

private:
    std::vector<std::pair<uint64_t, Any_T>> m_types_and_variables;
};

using GenericStack = BasicGenericStack<>;

}   // namespace kiln::util

namespace kiln::util {

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
BasicGenericStack<Any_T>::~BasicGenericStack()
{
    while (!m_types_and_variables.empty())
    {
        m_types_and_variables.pop_back();
    }
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <generic_stack_item_c Variable_T, typename... Args_T>
auto BasicGenericStack<Any_T>::emplace(Args_T&&... args) -> Variable_T&
{
    PRECOND(!contains<Variable_T>());

    return any_cast<Variable_T>(
        m_types_and_variables
            .emplace_back(
                std::piecewise_construct,
                std::tuple{ hash_u64<Variable_T>() },
                std::forward_as_tuple(
                    std::in_place_type<Variable_T>,
                    std::forward<Args_T>(args)...
                )
            )
            .second   //
    );
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <generic_stack_item_c Variable_T, typename Self_T>
auto BasicGenericStack<Any_T>::find(this Self_T& self) noexcept
    -> OptionalRef<const_like_t<Variable_T, Self_T>>
{
    const auto iter = std::ranges::find(
        self.m_types_and_variables, hash<Variable_T>(), &std::pair<uint64_t, Any_T>::first
    );
    if (iter == self.m_types_and_variables.cend())
    {
        return std::nullopt;
    }

    return OptionalRef<const_like_t<Variable_T, Self_T>>{
        any_cast<Variable_T>(iter->second)
    };
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <generic_stack_item_c Variable_T, typename Self_T>
auto BasicGenericStack<Any_T>::at(this Self_T&& self)
    -> forward_like_t<Variable_T, Self_T>
{
    auto found_variable{ std::forward<Self_T>(self).template find<Variable_T>() };

    PRECOND(
        found_variable.has_value(), std::format("Item {} not found", name_of<Variable_T>())
    );

    return std::forward_like<Self_T>(*found_variable);
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <generic_stack_item_c Variable_T>
auto BasicGenericStack<Any_T>::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_types_and_variables, hash_u64<Variable_T>(), &std::pair<uint64_t, Any_T>::first
    );
}

template <move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <typename Self_T, std::invocable<forward_like_t<Any_T, Self_T>> F>
    requires(
        std::is_const_v<std::remove_reference_t<Self_T>>
        || std::is_rvalue_reference_v<Self_T &&>
    )
auto BasicGenericStack<Any_T>::for_each(this Self_T&& self, F&& func) -> F
{
    for (auto&& [type, variable] : std::forward_like<Self_T>(self.m_types_and_variables))
    {
        std::invoke(func, std::forward_like<Self_T>(variable));
    }

    return std::forward<F>(func);
}

}   // namespace kiln::util
