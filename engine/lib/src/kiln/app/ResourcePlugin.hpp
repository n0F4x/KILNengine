#pragma once

#include <concepts>
#include <type_traits>

#include "kiln/app/App.hpp"
#include "kiln/util/GenericStack.hpp"

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
    auto insert_resource(Resource_T&& resource) -> void;

    template <resource_c Resource_T, typename... Args_T>
        requires std::constructible_from<Resource_T, Args_T&&...>
    auto emplace_resource(Args_T&&... args) -> void;

    template <decays_to_resource_injection_c Injection_T>
    auto inject_resource(Injection_T&& injection) -> void;

    auto operator()(App& app) && -> void;

private:
    template <typename Resource_T>
    struct SimpleResourceInjection {
        Resource_T resource;

        auto operator()() && -> Resource_T;
    };

    util::GenericStack m_resource_stack;
};

}   // namespace kiln::app

namespace kiln::app {

template <decays_to_resource_c Resource_T>
auto ResourcePlugin::insert_resource(Resource_T&& resource) -> void
{
    emplace_resource<Resource_T>(std::forward<Resource_T>(resource));
}

template <resource_c Resource_T, typename... Args_T>
    requires std::constructible_from<Resource_T, Args_T&&...>
auto ResourcePlugin::emplace_resource(Args_T&&... args) -> void
{
    inject_resource(SimpleResourceInjection<Resource_T>{ std::forward<Args_T>(args)... });
}

template <decays_to_resource_injection_c Injection_T>
auto ResourcePlugin::inject_resource(Injection_T&& injection) -> void
{
    m_resource_stack.inject(std::forward<Injection_T>(injection));
}

inline auto ResourcePlugin::operator()(App& app) && -> void
{
    PRECOND(
        app.resources().empty(),
        "Resource plugin should be called before adding any resource"
    );

    app.resources() = std::move(m_resource_stack);
}

template <typename Resource_T>
auto ResourcePlugin::SimpleResourceInjection<Resource_T>::operator()() && -> Resource_T
{
    return resource;
}

}   // namespace kiln::app
