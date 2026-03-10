module;

#include <concepts>
#include <deque>
#include <memory_resource>
#include <type_traits>
#include <utility>

export module kiln.app.context.ContextBuilder;

import kiln.app.context.Context;
import kiln.app.context.context_variable_c;
import kiln.app.context.decays_to_context_variable_c;
import kiln.app.context.decays_to_context_variable_injection_c;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.Deleter;

namespace kiln::app {

export class ContextBuilder {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;

    ContextBuilder();
    explicit ContextBuilder(allocator_type);
    ContextBuilder(ContextBuilder&&, allocator_type);


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


    template <decays_to_context_variable_c ContextVariable_T>
    auto insert(ContextVariable_T&& context_variable) -> void;

    template <context_variable_c ContextVariable_T, typename... Args_T>
        requires std::constructible_from<ContextVariable_T, Args_T&&...>
    auto emplace(Args_T&&... args) -> void;

    template <decays_to_context_variable_injection_c Injection_T>
    auto inject(Injection_T&& injection) -> void;


    auto merge_into(Context& context) && -> void;

private:
    using ErasedContextVariableInjection = util::MoveOnlyFunction<auto(Context&) &&->void>;

    template <typename ContextVariableInjection_T>
    struct ContextVariableInjectionLambda {
        ContextVariableInjection_T context_variable_injection;

        auto operator()(Context& context) && -> void;
    };

    std::unique_ptr<std::pmr::monotonic_buffer_resource, util::Deleter> m_memory_resource;
    std::pmr::deque<ErasedContextVariableInjection>                     m_injections;
};

}   // namespace kiln::app

namespace kiln::app {

template <decays_to_context_variable_c ContextVariable_T>
auto ContextBuilder::insert(ContextVariable_T&& context_variable) -> void
{
    insert(ContextVariable_T(std::forward<ContextVariable_T>(context_variable)));
}

template <context_variable_c ContextVariable_T, typename... Args_T>
    requires std::constructible_from<ContextVariable_T, Args_T&&...>
auto ContextBuilder::emplace(Args_T&&... args) -> void
{
    inject(
        [x_context_variable = ContextVariable_T(std::forward<Args_T>(args)...)] mutable   //
        -> std::decay_t<ContextVariable_T>          //
        {
            return std::move(x_context_variable);   //
        }
    );
}

template <decays_to_context_variable_injection_c Injection_T>
auto ContextBuilder::inject(Injection_T&& injection) -> void
{
    m_injections.emplace_back(
        std::in_place_type<ContextVariableInjectionLambda<std::decay_t<Injection_T>>>,
        std::forward<Injection_T>(injection)
    );
}

ContextBuilder::ContextBuilder() : ContextBuilder{ std::pmr::get_default_resource() } {}

ContextBuilder::ContextBuilder(allocator_type allocator)
    : m_memory_resource{
          allocator.new_object<std::pmr::monotonic_buffer_resource>(allocator.resource()),
          util::Deleter{ allocator }
      },
      m_injections{ m_memory_resource.get() }
{
}

ContextBuilder::ContextBuilder(ContextBuilder&& other, allocator_type allocator)
    : m_memory_resource{
          allocator.new_object<std::pmr::monotonic_buffer_resource>(allocator.resource()),
          util::Deleter{ allocator }
      },
      m_injections{ std::move(other.m_injections), m_memory_resource.get() }
{
}

auto ContextBuilder::get_allocator() const -> allocator_type
{
    return m_memory_resource->upstream_resource();
}

inline auto ContextBuilder::merge_into(Context& context) && -> void
{
    for (ErasedContextVariableInjection& injection : m_injections)
    {
        std::move(injection)(context);
    }
}

template <typename ContextVariableInjection_T>
auto ContextBuilder::ContextVariableInjectionLambda<ContextVariableInjection_T>::
    operator()(Context& context) && -> void
{
    context.inject(std::move(context_variable_injection));
}

}   // namespace kiln::app
