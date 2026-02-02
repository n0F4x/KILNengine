module;

#include <concepts>
#include <type_traits>
#include <vector>

export module kiln.app.resource.ResourceInjectionStack;

import kiln.app.resource.decays_to_resource_c;
import kiln.app.resource.decays_to_resource_injection_c;
import kiln.app.resource.resource_c;
import kiln.app.resource.ResourceStack;
import kiln.util.Function;

namespace kiln::app {

export class ResourceInjectionStack {
public:
    template <decays_to_resource_c Resource_T>
    auto insert_resource(Resource_T&& resource) -> void;

    template <resource_c Resource_T, typename... Args_T>
        requires std::constructible_from<Resource_T, Args_T&&...>
    auto emplace_resource(Args_T&&... args) -> void;

    template <decays_to_resource_injection_c Injection_T>
    auto inject_resource(Injection_T&& injection) -> void;


    auto merge_into(ResourceStack& resource_stack) && -> void;

private:
    using ErasedResourceInjection = util::MoveOnlyFunction<auto(ResourceStack&) &&->void>;

    template <typename ResourceInjection_T>
    struct ResourceInjectionLambda {
        ResourceInjection_T resource_injection;

        auto operator()(ResourceStack& resource_stack) && -> void;
    };

    std::vector<ErasedResourceInjection> m_injections;
};

}   // namespace kiln::app

namespace kiln::app {

template <decays_to_resource_c Resource_T>
auto ResourceInjectionStack::insert_resource(Resource_T&& resource) -> void
{
    inject_resource(
        [x_resource = std::forward<Resource_T>(resource)] mutable   //
        -> std::decay_t<Resource_T>                                 //
        {
            return std::move(x_resource);                           //
        }
    );
}

template <resource_c Resource_T, typename... Args_T>
    requires std::constructible_from<Resource_T, Args_T&&...>
auto ResourceInjectionStack::emplace_resource(Args_T&&... args) -> void
{
    insert_resource(Resource_T(std::forward<Args_T>(args)...));
}

template <decays_to_resource_injection_c Injection_T>
auto ResourceInjectionStack::inject_resource(Injection_T&& injection) -> void
{
    m_injections.emplace_back(
        ResourceInjectionLambda{ std::forward<Injection_T>(injection) }
    );
}

inline auto ResourceInjectionStack::merge_into(ResourceStack& resource_stack) && -> void
{
    for (ErasedResourceInjection& injection : m_injections)
    {
        std::move(injection)(resource_stack);
    }
}

template <typename ResourceInjection_T>
auto ResourceInjectionStack::ResourceInjectionLambda<ResourceInjection_T>::operator()(
    ResourceStack& resource_stack
) && -> void
{
    resource_stack.inject(std::move(resource_injection));
}

}   // namespace kiln::app
