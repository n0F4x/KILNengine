module;

#include <concepts>
#include <functional>
#include <memory>
#include <memory_resource>
#include <utility>

export module kiln.app.Builder;

import kiln.app.App;
import kiln.app.memory.Arena;
import kiln.app.memory.MemoryPluginInjection;
import kiln.app.plugin.plugin_c;
import kiln.app.plugin.plugin_injection_c;
import kiln.app.plugin.PluginTree;
import kiln.app.context.context_variable_c;
import kiln.app.context.decays_to_context_variable_c;
import kiln.app.context.decays_to_context_variable_injection_c;
import kiln.util.Deleter;

namespace kiln::app {

export class Builder {
public:
    Builder();

    template <typename Self_T, decays_to_plugin_c Plugin_T>
    auto insert_plugin(this Self_T&&, Plugin_T&& plugin) -> Self_T&&;

    template <plugin_c Plugin_T, typename Self_T, typename... Args_T>
        requires std::constructible_from<Plugin_T, Args_T&&...>
    auto emplace_plugin(this Self_T&&, Args_T&&... args) -> Self_T&&;

    template <typename Self_T, decays_to_plugin_injection_c PluginInjection_T>
    auto inject_plugin(this Self_T&&, PluginInjection_T&& plugin_injection) -> Self_T&&;

    template <typename Self_T, typename Bundle_T>
        requires std::invocable<Bundle_T&&, Builder&>
    auto apply_bundle(this Self_T&&, Bundle_T&& bundle) -> Self_T&&;


    [[nodiscard]]
    auto build() && -> App;

private:
    Arena m_app_arena;
    Arena m_builder_arena;

    PluginTree m_plugin_tree{ &m_builder_arena.pool_resource() };
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, decays_to_plugin_c Plugin_T>
auto Builder::insert_plugin(this Self_T&& self, Plugin_T&& plugin) -> Self_T&&
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
auto Builder::emplace_plugin(this Self_T&& self, Args_T&&... args) -> Self_T&&
{
    return std::forward<Self_T>(self).insert_plugin(
        Plugin_T(std::forward<Args_T>(args)...)
    );
}

template <typename Self_T, decays_to_plugin_injection_c PluginInjection_T>
auto Builder::inject_plugin(this Self_T&& self, PluginInjection_T&& plugin_injection)
    -> Self_T&&
{
    self.Builder::m_plugin_tree.plug_in(
        std::forward_like<PluginInjection_T>(plugin_injection),
        self.Builder::m_builder_arena.transitive_resource()
    );

    return std::forward<Self_T>(self);
}

template <typename Self_T, typename Bundle_T>
    requires std::invocable<Bundle_T&&, Builder&>
auto Builder::apply_bundle(this Self_T&& self, Bundle_T&& bundle) -> Self_T&&
{
    std::invoke(std::forward<Bundle_T>(bundle), self);
    return std::forward<Self_T>(self);
}

inline Builder::Builder()
{
    m_plugin_tree.plug_in_meta(MemoryPluginInjection{ m_app_arena, m_builder_arena });
}

inline auto Builder::build() && -> App
{
    App result{ std::move(m_app_arena) };

    std::move(m_plugin_tree).invoke_plugins(result);

    return result;
}

}   // namespace kiln::app
