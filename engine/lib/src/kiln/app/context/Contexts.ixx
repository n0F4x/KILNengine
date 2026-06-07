module;

#include <functional>
#include <memory>
#include <utility>

export module kiln.app.context.Contexts;

import kiln.app.context.context_c;
import kiln.util.containers.GenericStack;
import kiln.util.containers.OptionalRef;
import kiln.util.type_traits.const_like;

namespace kiln::app {

export class Contexts {
public:
    // required for interfacing with the standard
    using allocator_type = std::pmr::polymorphic_allocator<>;


    explicit Contexts(const allocator_type& allocator = std::pmr::get_default_resource());


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <context_c Context_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <context_c Context_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Context_T, Self_T>>;

    template <context_c Context_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::const_like_t<Context_T, Self_T>&;


    template <decays_to_context_c Context_T>
    auto insert(Context_T&& context) -> Context_T&;

    template <context_c Context_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Context_T&;

    template <context_c Context_T, typename... Args_T>
        requires(std::is_constructible_v<Context_T, Args_T && ...>)
    auto try_emplace(Args_T&&... args) -> std::pair<Context_T&, bool>;

private:
    util::GenericStack m_generic_stack;
};

}   // namespace kiln::app

namespace kiln::app {

Contexts::Contexts(const allocator_type& allocator) : m_generic_stack{ allocator } {}

auto Contexts::get_allocator() const noexcept -> allocator_type
{
    return m_generic_stack.get_allocator();
}

template <context_c Context_T>
auto Contexts::contains() const noexcept -> bool
{
    return m_generic_stack.contains<Context_T>();
}

template <context_c Context_T, typename Self_T>
auto Contexts::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<Context_T, Self_T>>
{
    return self.Contexts::m_generic_stack.template find<Context_T>();
}

template <context_c Context_T, typename Self_T>
auto Contexts::at(this Self_T&& self) -> util::const_like_t<Context_T, Self_T>&
{
    return self.Contexts::m_generic_stack.template at<Context_T>();
}

template <decays_to_context_c Context_T>
auto Contexts::insert(Context_T&& context) -> Context_T&
{
    return m_generic_stack.insert(std::forward<Context_T>(context));
}

template <context_c Context_T, typename... Args_T>
auto Contexts::emplace(Args_T&&... args) -> Context_T&
{
    return m_generic_stack.emplace<Context_T>(std::forward<Args_T>(args)...);
}

template <context_c Context_T, typename... Args_T>
    requires(std::is_constructible_v<Context_T, Args_T && ...>)
auto Contexts::try_emplace(Args_T&&... args) -> std::pair<Context_T&, bool>
{
    return m_generic_stack.try_emplace<Context_T>(std::forward<Args_T>(args)...);
}

}   // namespace kiln::app
