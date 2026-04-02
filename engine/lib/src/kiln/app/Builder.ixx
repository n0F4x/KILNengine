module;

#include <concepts>
#include <functional>
#include <memory>
#include <memory_resource>
#include <utility>

export module kiln.app.Builder;

import kiln.app.App;
import kiln.app.config.Config;
import kiln.app.config.ConfigPlugin;
import kiln.app.memory.Arena;
import kiln.app.memory.ArenaPlugin;
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.app.plugin.PluginTree;
import kiln.app.plugin.PluginStack;

namespace kiln::app {

template <typename T>
concept explicitly_meta_plugin = meta_plugin_c<T> && not requires { &T::operator(); };

export class Builder {
public:
    explicit Builder(const Config& config = {});


    template <plugin_c Plugin_T, typename Self_T>
    auto use_plugin(this Self_T&&) -> Self_T&&;

    template <explicitly_meta_plugin Plugin_T, typename Self_T>
    auto use_meta_plugin(this Self_T&&) -> Self_T&&;


    template <typename Self_T, typename Bundle_T>
        requires std::invocable<Bundle_T&&, Builder&>
    auto apply_bundle(this Self_T&&, Bundle_T&& bundle) -> Self_T&&;


    [[nodiscard]]
    auto build() && -> App;

private:
    Arena m_arena;

    PluginTree m_plugin_tree{ m_arena.pool_allocator() };
};

}   // namespace kiln::app

namespace kiln::app {

Builder::Builder(const Config& config)
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        m_arena.make_transient_resource()
    };

    m_plugin_tree.insert_meta_plugin(ArenaPlugin{ m_arena }, transient_memory_resource);
    m_plugin_tree.insert_plugin(ConfigPlugin{ config }, transient_memory_resource);
}

template <plugin_c Plugin_T, typename Self_T>
auto Builder::use_plugin(this Self_T&& self) -> Self_T&&
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        self.Builder::m_arena.make_transient_resource()
    };

    self.Builder::m_plugin_tree.template plug_in<Plugin_T>(transient_memory_resource);

    return std::forward<Self_T>(self);
}

template <explicitly_meta_plugin Plugin_T, typename Self_T>
auto Builder::use_meta_plugin(this Self_T&& self) -> Self_T&&
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        self.Builder::m_arena.make_transient_resource()
    };

    self.Builder::m_plugin_tree.template plug_in<Plugin_T>(transient_memory_resource);

    return std::forward<Self_T>(self);
}

template <typename Self_T, typename Bundle_T>
    requires std::invocable<Bundle_T&&, Builder&>
auto Builder::apply_bundle(this Self_T&& self, Bundle_T&& bundle) -> Self_T&&
{
    std::invoke(std::forward<Bundle_T>(bundle), self);
    return std::forward<Self_T>(self);
}

auto Builder::build() && -> App
{
    App    result{ std::move(m_arena) };
    Arena& arena{ result.context().at<Arena>() };

    PluginStack plugin_stack{ arena.pool_allocator().resource() };

    std::pmr::monotonic_buffer_resource transient_memory_resource{
        arena.make_transient_resource()
    };
    std::move(m_plugin_tree)
        .build_plugins(result, std::move(plugin_stack), transient_memory_resource);

    return result;
}

}   // namespace kiln::app
