module;

#include <concepts>
#include <functional>
#include <memory>
#include <utility>

export module kiln.app.context.Context;

import kiln.app.context.context_variable_c;
import kiln.app.memory.Arena;
import kiln.util.concepts.strips_to;
import kiln.util.containers.GenericStack;
import kiln.util.containers.OptionalRef;
import kiln.util.reflection;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::app {

export class Context {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;


    template <context_variable_c ContextVariable_T>
    [[nodiscard]]
    constexpr static auto is_always_available() noexcept -> bool;
    [[nodiscard]]
    constexpr static auto is_always_available(uint64_t context_variable_type_hash) noexcept
        -> bool;


    Context(std::allocator_arg_t, const allocator_type&, Arena& arena);


    template <context_variable_c ContextVariable_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <context_variable_c ContextVariable_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<ContextVariable_T, Self_T>>
        requires(!is_always_available<ContextVariable_T>());
    template <std::same_as<Arena>, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Arena, Self_T>>;

    template <context_variable_c ContextVariable_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::const_like_t<ContextVariable_T, Self_T>&
        requires(!is_always_available<ContextVariable_T>());
    template <std::same_as<Arena>, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::const_like_t<Arena, Self_T>&;


    template <decays_to_context_variable_c ContextVariable_T>
    auto insert(ContextVariable_T&& context_variable) -> ContextVariable_T&
        requires(!is_always_available<ContextVariable_T>());

    template <context_variable_c ContextVariable_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> ContextVariable_T&
        requires(!is_always_available<ContextVariable_T>());

private:
    util::GenericStack            m_generic_stack;
    std::reference_wrapper<Arena> m_arena_ref;
};

}   // namespace kiln::app

namespace kiln::app {

template <context_variable_c ContextVariable_T>
constexpr auto Context::is_always_available() noexcept -> bool
{
    return std::same_as<ContextVariable_T, Arena>;
}

constexpr auto Context::is_always_available(
    const uint64_t context_variable_type_hash
) noexcept -> bool
{
    return context_variable_type_hash == util::hash_u64<Arena>();
}

inline Context::Context(std::allocator_arg_t, const allocator_type& allocator, Arena& arena)
    : m_generic_stack{ allocator },
      m_arena_ref{ arena }
{
}

template <context_variable_c ContextVariable_T>
auto Context::contains() const noexcept -> bool
{
    if constexpr (is_always_available<ContextVariable_T>())
    {
        return true;
    }
    else
    {
        return m_generic_stack.contains<ContextVariable_T>();
    }
}

template <context_variable_c ContextVariable_T, typename Self_T>
auto Context::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<ContextVariable_T, Self_T>>
    requires(!is_always_available<ContextVariable_T>())
{
    return self.Context::m_generic_stack.template find<ContextVariable_T>();
}

template <std::same_as<Arena>, typename Self_T>
auto Context::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<Arena, Self_T>>
{
    return self.Context::m_arena_ref;
}

template <context_variable_c ContextVariable_T, typename Self_T>
auto Context::at(this Self_T&& self) -> util::const_like_t<ContextVariable_T, Self_T>&
    requires(!is_always_available<ContextVariable_T>())
{
    return self.Context::m_generic_stack.template at<ContextVariable_T>();
}

template <std::same_as<Arena>, typename Self_T>
auto Context::at(this Self_T&& self) -> util::const_like_t<Arena, Self_T>&
{
    return self.Context::m_arena_ref;
}

template <decays_to_context_variable_c ContextVariable_T>
auto Context::insert(ContextVariable_T&& context_variable) -> ContextVariable_T&
    requires(!is_always_available<ContextVariable_T>())
{
    return m_generic_stack.insert(std::forward<ContextVariable_T>(context_variable));
}

template <context_variable_c ContextVariable_T, typename... Args_T>
auto Context::emplace(Args_T&&... args) -> ContextVariable_T&
    requires(!is_always_available<ContextVariable_T>())
{
    return m_generic_stack.emplace<ContextVariable_T>(std::forward<Args_T>(args)...);
}

}   // namespace kiln::app
