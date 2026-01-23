#pragma once

#include <concepts>
#include <format>
#include <functional>
#include <type_traits>
#include <utility>

#include "kiln/app/App.hpp"
#include "kiln/app/ResourcePlugin.hpp"
#include "kiln/util/contract_macros.hpp"
#include "kiln/util/GenericStack.hpp"
#include "kiln/util/reflection.hpp"
#include "kiln/util/type_traits/arguments_of.hpp"
#include "kiln/util/type_traits/result_of.hpp"
#include "kiln/util/TypeList.hpp"

namespace kiln::app {

using ErasedPlugin = util::MoveOnlyFunction<void(App&) &&, 0>;

template <typename T>
concept plugin_c = util::basic_generic_stack_item_c<T, ErasedPlugin>;

template <typename T>
concept plugin_injection_c = plugin_c<util::result_of_t<T&&>>;

class Builder {
public:
    template <typename Self_T, util::decays_to_generic_stack_item_c Resource_T>
    auto insert_resource(this Self_T&&, Resource_T&& resource) -> Self_T;

    template <util::generic_stack_item_c Resource_T, typename Self_T, typename... Args_T>
        requires std::constructible_from<Resource_T, Args_T&&...>
    auto emplace_resource(this Self_T&&, Args_T&&... args) -> Self_T;

    template <typename Self_T, util::decays_to_generic_stack_item_injection_c Injection_T>
    auto inject_resource(this Self_T&&, Injection_T&& injection) -> Self_T;

    template <typename Self_T, plugin_injection_c PluginInjection_T>
    auto plug_in(this Self_T&&, PluginInjection_T&& plugin_injection) -> Self_T;

    [[nodiscard]]
    auto build() && -> App;

private:
    util::BasicGenericStack<ErasedPlugin>  m_plugins{ ResourcePlugin{} };
    std::reference_wrapper<ResourcePlugin> m_resource_plugin_ref{
        m_plugins.at<ResourcePlugin>()
    };
};

[[nodiscard]]
auto create() -> Builder;

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, util::decays_to_generic_stack_item_c Resource_T>
auto Builder::insert_resource(this Self_T&& self, Resource_T&& resource) -> Self_T
{
    self.Builder::m_resource_plugin_ref.get().insert_resource(
        std::forward<Resource_T>(resource)
    );
    return std::forward<Self_T>(self);
}

template <util::generic_stack_item_c Resource_T, typename Self_T, typename... Args_T>
    requires std::constructible_from<Resource_T, Args_T&&...>
auto Builder::emplace_resource(this Self_T&& self, Args_T&&... args) -> Self_T
{
    self.Builder::m_resource_plugin_ref.get().template emplace_resource<Resource_T>(
        std::forward<Args_T>(args)...
    );
    return std::forward<Self_T>(self);
}

template <typename Self_T, util::decays_to_generic_stack_item_injection_c Injection_T>
auto Builder::inject_resource(this Self_T&& self, Injection_T&& injection) -> Self_T
{
    self.Builder::m_resource_plugin_ref.get().inject_resource(
        std::forward<Injection_T>(injection)
    );
    return std::forward<Self_T>(self);
}

template <typename Self_T, plugin_injection_c PluginInjection_T>
auto Builder::plug_in(this Self_T&& self, PluginInjection_T&& plugin_injection) -> Self_T
{
    self.Builder::m_plugins.insert(
        [&self, &plugin_injection]<typename... Dependencies_T>(
            util::TypeList<Dependencies_T...>
        ) -> util::result_of_t<PluginInjection_T>   //
        {
            (PRECOND(
                 self.Builder::m_plugins
                     .template contains<std::remove_cvref_t<Dependencies_T>>(),
                 std::format(
                     "Plugin dependency of type `{}` not found",
                     util::name_of<std::remove_cvref_t<Dependencies_T>>()
                 )
             ),
             ...);

            return std::invoke(
                std::forward<PluginInjection_T>(plugin_injection),
                self.Builder::m_plugins.template at<std::remove_cvref_t<Dependencies_T>>()...
            );
        }(util::arguments_of_t<PluginInjection_T>{})
    );

    return std::forward<Self_T>(self);
}

inline auto Builder::build() && -> App
{
    App result{};

    std::move(m_plugins).for_each([&result](ErasedPlugin&& erased_plugin) -> void {
        std::move(erased_plugin)(result);
    });

    return result;
}

inline auto create() -> Builder
{
    return Builder{};
}

}   // namespace kiln::app
