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

namespace kiln::app {

template <typename T>
concept context_variable_c = util::storable_c<T> && util::naked_c<T>;

/*
 * References to contained items are valid until the context is alive.
 * Destroying a "moved-from" context does not invalidate any references to items.
 */
template <util::move_only_any_c Any_T = util::BasicMoveOnlyAny<0>>
    requires(Any_T::size == 0)
class BasicContext {
public:
    BasicContext()                        = default;
    BasicContext(const BasicContext&)     = delete;
    BasicContext(BasicContext&&) noexcept = default;
    ~BasicContext();

    auto operator=(const BasicContext&) -> BasicContext&     = delete;
    auto operator=(BasicContext&&) noexcept -> BasicContext& = default;

    template <context_variable_c Variable_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Variable_T&;

    template <context_variable_c Variable_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Variable_T, Self_T>>;

    template <context_variable_c Variable_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::forward_like_t<Variable_T, Self_T>;

    template <context_variable_c Variable_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <typename Self_T, std::invocable<util::forward_like_t<Any_T, Self_T>> F>
        requires(
            std::is_const_v<std::remove_reference_t<Self_T>>
            || std::is_rvalue_reference_v<Self_T &&>
        )
    auto for_each(this Self_T&&, F&& func) -> F;

private:
    std::vector<std::pair<uint64_t, Any_T>> m_types_and_variables;
};

using Context = BasicContext<>;

}   // namespace kiln::app

namespace kiln::app {

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
BasicContext<Any_T>::~BasicContext()
{
    while (!m_types_and_variables.empty())
    {
        m_types_and_variables.pop_back();
    }
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <context_variable_c Variable_T, typename... Args_T>
auto BasicContext<Any_T>::emplace(Args_T&&... args) -> Variable_T&
{
    PRECOND(!contains<Variable_T>());

    return util::any_cast<Variable_T>(
        m_types_and_variables
            .emplace_back(
                std::piecewise_construct,
                std::tuple{ util::hash_u64<Variable_T>() },
                std::forward_as_tuple(
                    std::in_place_type<Variable_T>,
                    std::forward<Args_T>(args)...
                )
            )
            .second   //
    );
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <context_variable_c Variable_T, typename Self_T>
auto BasicContext<Any_T>::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<Variable_T, Self_T>>
{
    const auto iter = std::ranges::find(
        self.m_types_and_variables,
        util::hash<Variable_T>(),
        &std::pair<uint64_t, Any_T>::first
    );
    if (iter == self.m_types_and_variables.cend())
    {
        return std::nullopt;
    }

    return util::OptionalRef<util::const_like_t<Variable_T, Self_T>>{
        util::any_cast<Variable_T>(iter->second)
    };
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <context_variable_c Variable_T, typename Self_T>
auto BasicContext<Any_T>::at(this Self_T&& self)
    -> util::forward_like_t<Variable_T, Self_T>
{
    auto found_variable{ std::forward<Self_T>(self).template find<Variable_T>() };

    PRECOND(
        found_variable.has_value(),
        std::format("Item {} not found", util::name_of<Variable_T>())
    );

    return std::forward_like<Self_T>(*found_variable);
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <context_variable_c Variable_T>
auto BasicContext<Any_T>::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_types_and_variables,
        util::hash_u64<Variable_T>(),
        &std::pair<uint64_t, Any_T>::first
    );
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <typename Self_T, std::invocable<util::forward_like_t<Any_T, Self_T>> F>
    requires(
        std::is_const_v<std::remove_reference_t<Self_T>>
        || std::is_rvalue_reference_v<Self_T &&>
    )
auto BasicContext<Any_T>::for_each(this Self_T&& self, F&& func) -> F
{
    for (auto&& [type, variable] : std::forward_like<Self_T>(self.m_types_and_variables))
    {
        std::invoke(func, std::forward_like<Self_T>(variable));
    }

    return std::forward<F>(func);
}

}   // namespace kiln::app
