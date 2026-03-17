module;

#include <concepts>
#include <functional>
#include <memory>
#include <utility>

export module kiln.app.context.Context;

import kiln.app.context.context_variable_c;
import kiln.app.memory.Arena;
import kiln.util.concepts.strips_to;
import kiln.util.GenericStack;
import kiln.util.OptionalRef;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::app {

export class Context {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;


    Context(std::allocator_arg_t, const allocator_type&, Arena& arena);


    template <context_variable_c ContextVariable_T>
        requires(!std::same_as<ContextVariable_T, Arena>)
    [[nodiscard]]
    auto contains() const noexcept -> bool;
    template <std::same_as<Arena>>
    [[nodiscard]]
    static auto contains() noexcept -> bool;

    template <context_variable_c ContextVariable_T, typename Self_T>
        requires(!std::same_as<ContextVariable_T, Arena>)
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<ContextVariable_T, Self_T>>;
    template <std::same_as<Arena>, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Arena, Self_T>> = delete (
            "Arena is always available"
        );

    template <context_variable_c ContextVariable_T, typename Self_T>
        requires(!std::same_as<ContextVariable_T, Arena>)
    [[nodiscard]]
    auto at(this Self_T&&) -> util::const_like_t<ContextVariable_T, Self_T>&;
    template <std::same_as<Arena>, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::const_like_t<Arena, Self_T>&;


    template <decays_to_context_variable_c ContextVariable_T>
        requires(!std::same_as<std::decay_t<ContextVariable_T>, Arena>)
    auto insert(ContextVariable_T&& context_variable) -> ContextVariable_T&;
    template <typename Arena_T>
        requires(util::strips_to_c<Arena_T, Arena>)
    auto insert(Arena&& context_variable)
        -> Arena& = delete ("Arena is always available");

    template <context_variable_c ContextVariable_T, typename... Args_T>
        requires(!std::same_as<ContextVariable_T, Arena>)
    auto emplace(Args_T&&... args) -> ContextVariable_T&;
    template <std::same_as<Arena>, typename... Args_T>
    auto emplace(Args_T&&... args) -> Arena& = delete ("Arena is always available");

private:
    util::GenericStack            m_generic_stack;
    std::reference_wrapper<Arena> m_arena_ref;
};

}   // namespace kiln::app

namespace kiln::app {

inline Context::Context(std::allocator_arg_t, const allocator_type& allocator, Arena& arena)
    : m_generic_stack{ allocator },
      m_arena_ref{ arena }
{
}

template <context_variable_c ContextVariable_T>
    requires(!std::same_as<ContextVariable_T, Arena>)
auto Context::contains() const noexcept -> bool
{
    return m_generic_stack.contains<ContextVariable_T>();
}

template <std::same_as<Arena>>
auto Context::contains() noexcept -> bool
{
    return true;
}

template <context_variable_c ContextVariable_T, typename Self_T>
    requires(!std::same_as<ContextVariable_T, Arena>)
auto Context::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<ContextVariable_T, Self_T>>
{
    return self.Context::m_generic_stack.template find<ContextVariable_T>();
}

template <context_variable_c ContextVariable_T, typename Self_T>
    requires(!std::same_as<ContextVariable_T, Arena>)
auto Context::at(this Self_T&& self) -> util::const_like_t<ContextVariable_T, Self_T>&
{
    return self.Context::m_generic_stack.template at<ContextVariable_T>();
}

template <std::same_as<Arena>, typename Self_T>
auto Context::at(this Self_T&& self) -> util::const_like_t<Arena, Self_T>&
{
    return self.Context::m_arena_ref;
}

template <decays_to_context_variable_c ContextVariable_T>
    requires(!std::same_as<std::decay_t<ContextVariable_T>, Arena>)
auto Context::insert(ContextVariable_T&& context_variable) -> ContextVariable_T&
{
    return m_generic_stack.insert(std::forward<ContextVariable_T>(context_variable));
}

template <context_variable_c ContextVariable_T, typename... Args_T>
    requires(!std::same_as<ContextVariable_T, Arena>)
auto Context::emplace(Args_T&&... args) -> ContextVariable_T&
{
    return m_generic_stack.emplace<ContextVariable_T>(std::forward<Args_T>(args)...);
}

}   // namespace kiln::app
