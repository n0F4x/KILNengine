#pragma once

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include "kiln/app/Context.hpp"
#include "kiln/util/concepts/type_list_all_of.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/Function.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits/arguments_of.hpp"
#include "kiln/util/type_traits/result_of.hpp"

namespace kiln::app {

namespace internal {

template <typename T>
struct IsContextVariableDependency
    : std::bool_constant<context_variable_c<std::remove_cvref_t<T>>> {};

}   // namespace internal

template <typename T>
concept context_variable_injection_c =
    context_variable_c<util::result_of_t<T>>
    && util::
        type_list_all_of_c<util::arguments_of_t<T>, internal::IsContextVariableDependency>;

template <typename T>
concept decays_to_context_variable_injection_c =
    context_variable_injection_c<std::decay_t<T>>;

class ContextBuilder {
public:
    template <decays_to_context_variable_injection_c Injection_T>
    auto inject(Injection_T&& injection) -> std::decay_t<Injection_T>&;

    [[nodiscard]]
    auto build() && -> Context;

private:
    using Injection = util::MoveOnlyFunction<void(Context&) &&, 0>;

    template <typename DecayedInjection_T, typename Variable_T>
    struct WrappedInjection {
        DecayedInjection_T injection;

        auto operator()(Context& context) && -> void;
    };

    BasicContext<Injection> m_injections;
    std::vector<uint64_t>   m_variable_types;

    template <context_variable_c Variable_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Injection_T>
auto apply_injection(Injection_T&& injection, Context& context)
    -> util::result_of_t<Injection_T>
{
    using Parameters = util::arguments_of_t<Injection_T>;
    static_assert(util::type_list_all_of_c<Parameters, std::is_lvalue_reference>);

    return [&injection,
            &context]<typename... Parameters_T>(util::TypeList<Parameters_T...>) {
        (PRECOND(
             (context.contains<std::remove_cvref_t<Parameters_T>>()),
             std::format(
                 "Missing dependency `{}` for injection `{}`",
                 util::name_of<std::remove_cvref_t<Parameters_T>>(),
                 util::name_of<Injection_T>()
             )
         ),
         ...);

        return std::invoke(
            std::forward<Injection_T>(injection),   //
            [&context]<typename Parameter_T> -> decltype(auto) {
                return context.at<std::remove_cvref_t<Parameter_T>>();
            }.template operator()<Parameters_T>()...
        );
    }(Parameters{});
}

template <decays_to_context_variable_injection_c Injection_T>
auto ContextBuilder::inject(Injection_T&& injection) -> std::decay_t<Injection_T>&
{
    using Variable         = std::remove_cvref_t<util::result_of_t<Injection_T>>;
    using DecayedInjection = std::decay_t<Injection_T>;
    using WrappedInjection = WrappedInjection<DecayedInjection, Variable>;

    PRECOND((!contains<Variable>()));

    WrappedInjection& wrapped_injection = m_injections.emplace<WrappedInjection>(
        WrappedInjection{ std::forward<Injection_T>(injection) }
    );

    m_variable_types.push_back(util::hash_u64<Variable>());

    return wrapped_injection.injection;
}

template <typename DecayedInjection_T, typename Variable_T>
auto ContextBuilder::WrappedInjection<DecayedInjection_T, Variable_T>::operator()(
    Context& context
) && -> void
{
    context.emplace<Variable_T>(apply_injection(std::move(injection), context));
}

inline auto ContextBuilder::build() && -> Context
{
    Context result;

    std::move(m_injections).for_each([&result](Injection&& injection) -> void {
        std::move(injection)(result);
    });

    return result;
}

template <context_variable_c Variable_T>
auto ContextBuilder::contains() const noexcept -> bool
{
    return std::ranges::contains(m_variable_types, util::hash_u64<Variable_T>());
}

}   // namespace kiln::app
