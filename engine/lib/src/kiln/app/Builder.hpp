#pragma once

#include <concepts>
#include <utility>

#include "kiln/app/App.hpp"
#include "kiln/app/PluginTree.hpp"
#include "kiln/app/ResourceInjectionStack.hpp"

namespace kiln::app {

class Builder {
public:
    template <typename Self_T, decays_to_resource_c Resource_T>
    auto insert_resource(this Self_T&&, Resource_T&& resource) -> Self_T;

    template <resource_c Resource_T, typename Self_T, typename... Args_T>
        requires std::constructible_from<Resource_T, Args_T&&...>
    auto emplace_resource(this Self_T&&, Args_T&&... args) -> Self_T;

    template <typename Self_T, decays_to_resource_injection_c Injection_T>
    auto inject_resource(this Self_T&&, Injection_T&& injection) -> Self_T;


    template <typename Self_T, decays_to_plugin_c Plugin_T>
    auto insert_plugin(this Self_T&&, Plugin_T&& plugin) -> Self_T;

    template <plugin_c Plugin_T, typename Self_T, typename... Args_T>
        requires std::constructible_from<Plugin_T, Args_T&&...>
    auto emplace_plugin(this Self_T&&, Args_T&&... args) -> Self_T;

    template <typename Self_T, decays_to_plugin_injection_c PluginInjection_T>
    auto inject_plugin(this Self_T&&, PluginInjection_T&& plugin_injection) -> Self_T;


    [[nodiscard]]
    auto build() && -> App;

private:
    PluginTree             m_plugin_tree;
    ResourceInjectionStack m_resource_injection_stack;
};

[[nodiscard]]
auto create() -> Builder;

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, decays_to_resource_c Resource_T>
auto Builder::insert_resource(this Self_T&& self, Resource_T&& resource) -> Self_T
{
    self.m_resource_injection_stack.insert_resource(std::forward<Resource_T>(resource));
    return std::forward<Self_T>(self);
}

template <resource_c Resource_T, typename Self_T, typename... Args_T>
    requires std::constructible_from<Resource_T, Args_T&&...>
auto Builder::emplace_resource(this Self_T&& self, Args_T&&... args) -> Self_T
{
    self.m_resource_injection_stack.template emplace_resource<Resource_T>(
        std::forward<Args_T>(args)...
    );
    return std::forward<Self_T>(self);
}

template <typename Self_T, decays_to_resource_injection_c Injection_T>
auto Builder::inject_resource(this Self_T&& self, Injection_T&& injection) -> Self_T
{
    self.m_resource_injection_stack.inject_resource(std::forward<Injection_T>(injection));
    return std::forward<Self_T>(self);
}

template <typename Self_T, decays_to_plugin_c Plugin_T>
auto Builder::insert_plugin(this Self_T&& self, Plugin_T&& plugin) -> Self_T
{
    return std::forward<Self_T>(self).inject_plugin(
        [x_plugin = std::forward<Plugin_T>(plugin)] mutable -> std::decay_t<Plugin_T>
        {
            return std::move(x_plugin);   //
        }
    );
}

template <plugin_c Plugin_T, typename Self_T, typename... Args_T>
    requires std::constructible_from<Plugin_T, Args_T&&...>
auto Builder::emplace_plugin(this Self_T&& self, Args_T&&... args) -> Self_T
{
    return std::forward<Self_T>(self).insert_plugin(
        Plugin_T(std::forward<Args_T>(args)...)
    );
}

template <typename Self_T, decays_to_plugin_injection_c PluginInjection_T>
auto Builder::inject_plugin(this Self_T&& self, PluginInjection_T&& plugin_injection)
    -> Self_T
{
    self.m_plugin_tree.plug_in(std::forward_like<PluginInjection_T>(plugin_injection));

    return std::forward<Self_T>(self);
}

inline auto Builder::build() && -> App
{
    App result{};

    std::move(m_plugin_tree).invoke_plugins(result);
    std::move(m_resource_injection_stack).merge_into(result.resources());

    return result;
}

inline auto create() -> Builder
{
    return Builder{};
}

}   // namespace kiln::app
