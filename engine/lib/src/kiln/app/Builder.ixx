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
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.meta_plugin_injection_c;
import kiln.app.plugin.plugin_c;
import kiln.app.plugin.plugin_injection_c;
import kiln.app.plugin.PluginTree;

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


    template <typename Self_T, decays_to_meta_plugin_c MetaPlugin_T>
    auto insert_meta_plugin(this Self_T&&, MetaPlugin_T&& meta_plugin) -> Self_T&&;

    template <meta_plugin_c MetaPlugin_T, typename Self_T, typename... Args_T>
        requires std::constructible_from<MetaPlugin_T, Args_T&&...>
    auto emplace_meta_plugin(this Self_T&&, Args_T&&... args) -> Self_T&&;

    template <typename Self_T, decays_to_meta_plugin_injection_c MetaPluginInjection_T>
    auto inject_meta_plugin(this Self_T&&, MetaPluginInjection_T&& meta_plugin_injection)
        -> Self_T&&;


    template <typename Self_T, typename Bundle_T>
        requires std::invocable<Bundle_T&&, Builder&>
    auto apply_bundle(this Self_T&&, Bundle_T&& bundle) -> Self_T&&;


    [[nodiscard]]
    auto build() && -> App;

private:
    Arena m_app_arena;
    Arena m_builder_arena;

    PluginTree m_plugin_tree{ m_builder_arena.pool_allocator() };
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, decays_to_plugin_c Plugin_T>
auto Builder::insert_plugin(this Self_T&& self, Plugin_T&& plugin) -> Self_T&&
{
    return std::forward<Self_T>(self)
        .template emplace_plugin<std::remove_cvref_t<Plugin_T>>(
            std::forward<Plugin_T>(plugin)
        );
}

template <plugin_c Plugin_T, typename Self_T, typename... Args_T>
    requires std::constructible_from<Plugin_T, Args_T&&...>
auto Builder::emplace_plugin(this Self_T&& self, Args_T&&... args) -> Self_T&&
{
    return std::forward<Self_T>(self).inject_plugin(
        [x_plugin =
             Plugin_T(std::forward<Args_T>(args)...)] mutable -> std::decay_t<Plugin_T>
        {
            return std::move(x_plugin);   //
        }
    );
}

template <typename Self_T, decays_to_plugin_injection_c PluginInjection_T>
auto Builder::inject_plugin(this Self_T&& self, PluginInjection_T&& plugin_injection)
    -> Self_T&&
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        self.Builder::m_builder_arena.make_transient_resource()
    };

    self.Builder::m_plugin_tree.plug_in(
        std::forward_like<PluginInjection_T>(plugin_injection), transient_memory_resource
    );

    return std::forward<Self_T>(self);
}

template <typename Self_T, decays_to_meta_plugin_c MetaPlugin_T>
auto Builder::insert_meta_plugin(this Self_T&& self, MetaPlugin_T&& meta_plugin)
    -> Self_T&&
{
    return std::forward<Self_T>(self)
        .template emplace_meta_plugin<std::remove_cvref_t<MetaPlugin_T>>(
            std::forward<MetaPlugin_T>(meta_plugin)
        );
}

template <meta_plugin_c MetaPlugin_T, typename Self_T, typename... Args_T>
    requires std::constructible_from<MetaPlugin_T, Args_T&&...>
auto Builder::emplace_meta_plugin(this Self_T&& self, Args_T&&... args) -> Self_T&&
{
    return std::forward<Self_T>(self).inject_meta_plugin(
        [x_meta_plugin = MetaPlugin_T(std::forward<Args_T>(args)...)] mutable
            -> std::decay_t<MetaPlugin_T>
        {
            return std::move(x_meta_plugin);   //
        }
    );
}

template <typename Self_T, decays_to_meta_plugin_injection_c MetaPluginInjection_T>
auto Builder::inject_meta_plugin(
    this Self_T&&           self,
    MetaPluginInjection_T&& meta_plugin_injection
) -> Self_T&&
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        self.Builder::m_builder_arena.make_transient_resource()
    };

    self.Builder::m_plugin_tree.plug_in_meta(
        std::forward_like<MetaPluginInjection_T>(meta_plugin_injection),
        transient_memory_resource
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

    std::pmr::monotonic_buffer_resource transient_memory_resource{
        result.context().at<Arena>().make_transient_resource()
    };
    std::move(m_plugin_tree).build_plugins(result, transient_memory_resource);

    return result;
}

}   // namespace kiln::app
