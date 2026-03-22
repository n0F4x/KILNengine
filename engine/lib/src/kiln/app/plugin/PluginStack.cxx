module;

#include <algorithm>
#include <cassert>
#include <deque>
#include <format>
#include <memory_resource>
#include <string_view>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.app.plugin.PluginStack;

import kiln.app.plugin.ErasedPlugin;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;

namespace kiln::app {

PluginStack::PluginStack(const allocator_type& allocator)
    : m_plugin_hashes{ allocator },
      m_plugins{ allocator }
{
}

PluginStack::PluginStack(PluginStack&& other, const allocator_type& allocator)
    : m_plugin_hashes{ std::move(other.m_plugin_hashes), allocator },
      m_plugins{ std::move(other.m_plugins), allocator }
{
}

PluginStack::~PluginStack()
{
    while (!m_plugins.empty())
    {
        m_plugins.pop_back();
    }
}

auto PluginStack::get_allocator() const -> allocator_type
{
    return m_plugins.get_allocator();
}

auto PluginStack::build(
    App&                       app,
    std::pmr::memory_resource& transient_memory_resource
) && -> void
{
    check_for_configuration_dependencies();
    check_for_cyclic_dependencies();

    fix_order(transient_memory_resource);

    for (ErasedPlugin& plugin : m_plugins)
    {
        std::move(plugin).build(app);
    }
}

auto PluginStack::PluginNameChainNode::format() const -> std::string
{
    std::string result;
    format(result, 0);
    return result;
}

auto PluginStack::PluginNameChainNode::format(
    std::string&   out_string,
    const uint32_t capacity
) const -> void
{
    if (previous != nullptr)
    {
        previous->format(
            out_string,
            capacity + std::strlen(" -> ") + static_cast<uint32_t>(plugin_name.length())
        );
        out_string.append(" -> ");
    }
    else
    {
        assert(
            out_string.empty()
            && "this invocation should be the one to start building the string"
        );
        out_string.reserve(capacity + plugin_name.size());
    }

    out_string.append(std::format("{}", plugin_name));
}

auto PluginStack::check_for_configuration_dependencies() const -> void
{
    for (const ErasedPlugin& plugin : m_plugins)
    {
        check_for_configuration_dependencies(plugin);
    }
}

auto PluginStack::check_for_configuration_dependencies(
    const ErasedPlugin& erased_plugin
) const -> void
{
    for ([[maybe_unused]]
         const uint64_t configuration_dependency_hash :
         erased_plugin.configuration_dependency_hash_set())
    {
        PRECOND(
            find(configuration_dependency_hash).has_value(),
            std::format(
                "Missing configuration dependency for plugin `{}`", erased_plugin.name()
            )
        );
    }
}

auto PluginStack::check_for_cyclic_dependencies() const -> void
{
    for (const ErasedPlugin& plugin : m_plugins)
    {
        check_for_cyclic_dependencies(plugin);
    }
}

auto PluginStack::check_for_cyclic_dependencies(const ErasedPlugin& erased_plugin) const
    -> void
{
    for (const PluginNameChainNode plugin_name_chain_node{
             .plugin_name = erased_plugin.name(),
         };
         const uint64_t configuration_dependency_hash :
         erased_plugin.configuration_dependency_hash_set())
    {
        const ErasedPlugin& configuration_plugin{ at(configuration_dependency_hash) };
        check_for_cyclic_dependency(
            erased_plugin.hash(),
            erased_plugin.name(),
            configuration_plugin.name(),
            configuration_dependency_hash,
            plugin_name_chain_node
        );
    }
}

auto PluginStack::check_for_cyclic_dependency(
    const uint64_t             plugin_hash,
    const std::string_view     plugin_name,
    const std::string_view     configuration_plugin_name,
    const uint64_t             dependency_hash,
    const PluginNameChainNode& visited_plugin_names
) const -> void
{
    PRECOND(
        plugin_hash != dependency_hash,
        std::format(
            "Cyclic dependency detected through configuration:"   //
            " See `{}` being configured using `{}`"               //
            " ({} -> {})",
            plugin_name,
            configuration_plugin_name,
            visited_plugin_names.format(),
            plugin_name
        )
    );

    const auto dependency_iter =
        std::ranges::find(m_plugins, dependency_hash, &ErasedPlugin::hash);
    if (dependency_iter == m_plugins.cend())
    {
        return;
    }

    for (const uint64_t next_dependency_hash : dependency_iter->dependency_hash_set())
    {
        const PluginNameChainNode plugin_chain_node{
            .previous    = &visited_plugin_names,
            .plugin_name = dependency_iter->name(),
        };

        check_for_cyclic_dependency(
            plugin_hash,
            plugin_name,
            configuration_plugin_name,
            next_dependency_hash,
            plugin_chain_node
        );
    }
}

auto PluginStack::fix_order(std::pmr::memory_resource& transient_memory_resource) -> void
{
    for (const uint64_t plugin_hash : m_plugin_hashes)
    {
        fix_order(plugin_hash, transient_memory_resource);
    }
}

auto PluginStack::fix_order(
    const uint64_t             plugin_hash,
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    /*
     * Shift plugin and all its dependencies in front of the first dependent plugin
     */


    auto first_dependent_plugin_iter = std::ranges::find_if(
        m_plugins,
        [plugin_hash](const ErasedPlugin& dependent_plugin) -> bool
        {
            return std::ranges::binary_search(
                dependent_plugin.dependency_hash_set(), plugin_hash
            );
        }
    );
    if (first_dependent_plugin_iter == m_plugins.cend())
    {
        return;
    }


    const ErasedPlugin& plugin{ at(plugin_hash) };

    std::pmr::deque<uint64_t> all_dependency_plugin_hashes{
        std::initializer_list{ plugin_hash }, &transient_memory_resource
    };
    collect_all_resolved_dependency_plugin_hashes(plugin, all_dependency_plugin_hashes);
    std::ranges::sort(all_dependency_plugin_hashes);


    /*
     * Collect plugins that need to be shifted in order
     */

    std::pmr::vector<decltype(first_dependent_plugin_iter)> to_be_shifted_plugin_iters{
        &transient_memory_resource
    };
    to_be_shifted_plugin_iters.reserve(all_dependency_plugin_hashes.size());
    for (auto iter{ std::next(first_dependent_plugin_iter) };   //
         iter != m_plugins.cend();
         ++iter)
    {
        if (std::ranges::binary_search(all_dependency_plugin_hashes, iter->hash()))
        {
            to_be_shifted_plugin_iters.push_back(iter);
        }
    }


    for (const auto& plugin_iter : to_be_shifted_plugin_iters)
    {
        /*
         * Shift injection in front of the first dependent injection
         */

        // TODO: use std::ranges::rotate when libc++ supports it
        first_dependent_plugin_iter =
            std::rotate(first_dependent_plugin_iter, plugin_iter, std::next(plugin_iter));
    }
}

auto PluginStack::collect_all_resolved_dependency_plugin_hashes(
    const ErasedPlugin&        plugin,
    std::pmr::deque<uint64_t>& out
) const -> void
{
    for (const uint64_t plugin_dependency_hash : plugin.dependency_hash_set())
    {
        if (util::OptionalRef dependency{ find(plugin_dependency_hash) };
            dependency.has_value() && !std::ranges::contains(out, plugin_dependency_hash))
        {
            out.push_back(plugin_dependency_hash);
            collect_all_resolved_dependency_plugin_hashes(*dependency, out);
        }
    }
}

}   // namespace kiln::app
