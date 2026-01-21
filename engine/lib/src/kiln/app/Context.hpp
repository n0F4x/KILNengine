#pragma once

#include <cstdint>
#include <format>

#include <tsl/ordered_map.h>

#include "kiln/util/Any.hpp"
#include "kiln/util/concepts/naked.hpp"
#include "kiln/util/concepts/storable.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/OptionalRef.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits.hpp"

namespace kiln::app {

namespace internal {

template <typename T>
concept context_variable_c = util::storable_c<T> && util::naked_c<T>;

}   // namespace internal

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

    template <internal::context_variable_c Variable_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Variable_T&;

    template <internal::context_variable_c Variable_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Variable_T, Self_T>>;

    template <internal::context_variable_c Variable_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::forward_like_t<Variable_T, Self_T>;

    template <internal::context_variable_c Variable_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    tsl::ordered_map<uint64_t, Any_T> m_variables;
};

using Context = BasicContext<>;

}   // namespace kiln::app

namespace kiln::app {

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
BasicContext<Any_T>::~BasicContext()
{
    while (!m_variables.empty())
    {
        m_variables.pop_back();
    }
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <internal::context_variable_c Variable_T, typename... Args_T>
auto BasicContext<Any_T>::emplace(Args_T&&... args) -> Variable_T&
{
    return util::any_cast<Variable_T>(
        m_variables
            .try_emplace(
                util::hash<Variable_T>(),
                std::in_place_type<Variable_T>,
                std::forward<Args_T>(args)...
            )
            .first.value()   //
    );
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <internal::context_variable_c Variable_T, typename Self_T>
auto BasicContext<Any_T>::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<Variable_T, Self_T>>
{
    const auto iter{ self.m_variables.find(util::hash<Variable_T>()) };
    if (iter == self.m_variables.cend())
    {
        return std::nullopt;
    }

    return util::OptionalRef<util::const_like_t<Variable_T, Self_T>>{
        util::any_cast<Variable_T>(iter->second)
    };
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <internal::context_variable_c Variable_T, typename Self_T>
auto BasicContext<Any_T>::at(this Self_T&& self)
    -> util::forward_like_t<Variable_T, Self_T>
{
    PRECOND(
        self.template contains<Variable_T>(),
        std::format("Item {} not found", util::name_of<Variable_T>())
    );

    return util::any_cast<Variable_T>(
        std::forward_like<Self_T>(self.m_variables.at(util::hash<Variable_T>()))
    );
}

template <util::move_only_any_c Any_T>
    requires(Any_T::size == 0)
template <internal::context_variable_c Variable_T>
auto BasicContext<Any_T>::contains() const noexcept -> bool
{
    return m_variables.contains(util::hash<Variable_T>());
}

}   // namespace kiln::app
