#pragma once

#include <concepts>
#include <type_traits>

#include "kiln/app/App.hpp"
#include "kiln/util/GenericStack.hpp"
#include "kiln/util/type_traits/result_of.hpp"

namespace kiln::app {

template <typename T>
concept resource_c = util::generic_stack_item_c<T>;

template <typename T>
concept decays_to_resource_c = resource_c<std::decay_t<T>>;

template <typename T>
concept resource_injection_c = util::generic_stack_item_injection_c<T>;

template <typename T>
concept decays_to_resource_injection_c = resource_injection_c<std::decay_t<T>>;

class ResourcePlugin {
public:
    template <decays_to_resource_c Resource_T>
    auto insert_resource(Resource_T&& resource) -> std::decay_t<Resource_T>&;

    template <resource_c Resource_T, typename... Args_T>
        requires std::constructible_from<Resource_T, Args_T&&...>
    auto emplace_resource(Args_T&&... args) -> Resource_T&;

    template <decays_to_resource_injection_c Injection_T>
    auto inject_resource(Injection_T&& injection) -> util::result_of_t<Injection_T>&;


    auto operator()(App& app) && -> void;

private:
    util::GenericStack m_resource_stack;
};

}   // namespace kiln::app

namespace kiln::app {

template <decays_to_resource_c Resource_T>
auto ResourcePlugin::insert_resource(Resource_T&& resource) -> std::decay_t<Resource_T>&
{
    return m_resource_stack.insert(std::forward<Resource_T>(resource));
}

template <resource_c Resource_T, typename... Args_T>
    requires std::constructible_from<Resource_T, Args_T&&...>
auto ResourcePlugin::emplace_resource(Args_T&&... args) -> Resource_T&
{
    return m_resource_stack.emplace<Resource_T>(std::forward<Args_T>(args)...);
}

template <decays_to_resource_injection_c Injection_T>
auto ResourcePlugin::inject_resource(Injection_T&& injection)
    -> util::result_of_t<Injection_T>&
{
    return m_resource_stack.inject(std::forward<Injection_T>(injection));
}

inline auto ResourcePlugin::operator()(App& app) && -> void
{
    PRECOND(
        app.resources().empty(),
        "Resource plugin should be called before adding any resource to app"
    );

    app.resources() = std::move(m_resource_stack);
}

}   // namespace kiln::app
