module;

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>
#include <format>
#include <memory_resource>
#include <ranges>
#include <string>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.app.plugin.PluginTree;

import kiln.util.contracts;

namespace kiln::app {

namespace internal {

ErasedPluginInjection::ErasedPluginInjection(
    ErasedPluginInjection&& other,
    const allocator_type&   allocator
)
    : m_function{ std::move(other.m_function), allocator },
      m_hash{ std::move(other.m_hash) },
      m_name{ std::move(other.m_name) },
      m_unresolved_and_resolved_dependency_hashes{
          std::move(other.m_unresolved_and_resolved_dependency_hashes),
          allocator
      },
      m_unresolved_dependency_hash_count{
          std::move(other.m_unresolved_dependency_hash_count)
      }
{
}

auto ErasedPluginInjection::get_allocator() const -> allocator_type
{
    return m_function.get_allocator();
}

auto ErasedPluginInjection::hash() const noexcept -> uint64_t
{
    return m_hash;
}

auto ErasedPluginInjection::name() const noexcept -> std::string_view
{
    return m_name;
}

auto ErasedPluginInjection::dependency_hashes() const noexcept
    -> std::span<const uint64_t>
{
    return m_unresolved_and_resolved_dependency_hashes;
}

auto ErasedPluginInjection::resolved_dependency_hash_set() const noexcept
    -> std::span<const uint64_t>
{
    return std::views::drop(
        m_unresolved_and_resolved_dependency_hashes, m_unresolved_dependency_hash_count
    );
}

auto ErasedPluginInjection::unresolved_dependency_hash_set() const noexcept
    -> std::span<const uint64_t>
{
    return std::views::take(
        m_unresolved_and_resolved_dependency_hashes, m_unresolved_dependency_hash_count
    );
}

auto ErasedPluginInjection::resolve_dependency(const uint64_t new_plugin_hash) -> void
{
    if (m_unresolved_dependency_hash_count == 0)
    {
        return;
    }

    const auto unresolved_hashes_view{
        std::views::take(
            m_unresolved_and_resolved_dependency_hashes,
            m_unresolved_dependency_hash_count
        )   //
    };

    const auto iter = std::ranges::find(unresolved_hashes_view, new_plugin_hash);
    if (iter == std::ranges::cend(unresolved_hashes_view))
    {
        return;
    }

    if (std::ranges::distance(iter, unresolved_hashes_view.end()) > 1)
    {
        // TODO: use std::ranges::rotate when libc++ supports it
        std::rotate(iter, std::next(iter), unresolved_hashes_view.end());
    }
    --m_unresolved_dependency_hash_count;

    std::ranges::sort(
        std::views::drop(
            m_unresolved_and_resolved_dependency_hashes, m_unresolved_dependency_hash_count
        )
    );
}

auto ErasedPluginInjection::operator()(PluginStack& plugin_stack) && -> void
{
    std::move(m_function)(plugin_stack, resolved_dependency_hash_set());
}

}   // namespace internal

PluginTree::PluginTree(const allocator_type& allocator)
    : m_resource_allocator{ allocator },
      m_injections_resource{ m_resource_allocator.resource() },
      m_plugin_injections{ &m_injections_resource },
      m_unresolved_plugin_hashes_resource{ m_resource_allocator.resource() },
      m_unresolved_optional_dependency_hashes{ &m_unresolved_plugin_hashes_resource }
{
}

auto PluginTree::build_plugins(
    App&                       app,
    std::pmr::memory_resource& transitive_memory_resource
) && -> void
{
    PluginStack plugin_stack;

    for (internal::ErasedPluginInjection& plugin_injection : m_plugin_injections)
    {
        std::move(plugin_injection)(plugin_stack);
    }

    std::move(plugin_stack).build(app, transitive_memory_resource);
}

auto PluginTree::PluginNameChainNode::format() const -> std::string
{
    std::string result;
    format(result, 0);
    return result;
}

auto PluginTree::PluginNameChainNode::format(
    std::string&      out_string,
    const std::size_t capacity
) const -> void
{
    if (previous != nullptr)
    {
        previous->format(
            out_string, capacity + std::strlen(" -> ") + plugin_name.length()
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

auto PluginTree::check_for_new_cyclic_dependency(
    const uint64_t             new_plugin_hash,
    const std::string_view     new_plugin_name,
    const uint64_t             dependency_hash,
    const PluginNameChainNode& visited_plugin_names
) const -> void
{
    PRECOND(
        new_plugin_hash != dependency_hash,
        std::format(
            "Cyclic dependency detected - plugin of type `{}` depends on itself "   //
            "({} -> {})",
            new_plugin_name,
            visited_plugin_names.format(),
            new_plugin_name
        )
    );

    const auto dependency_iter = std::ranges::find(
        m_plugin_injections, dependency_hash, &internal::ErasedPluginInjection::hash
    );
    if (dependency_iter == m_plugin_injections.cend())
    {
        return;
    }

    for (const uint64_t next_dependency_hash : dependency_iter->dependency_hashes())
    {
        const PluginNameChainNode plugin_chain_node{
            .previous    = &visited_plugin_names,
            .plugin_name = dependency_iter->name(),
        };

        check_for_new_cyclic_dependency(
            new_plugin_hash, new_plugin_name, next_dependency_hash, plugin_chain_node
        );
    }
}

auto PluginTree::collect_all_resolved_dependency_hashes(
    const internal::ErasedPluginInjection& plugin_injection,
    std::pmr::deque<uint64_t>&             out
) const -> void
{
    for (const uint64_t dependency_hash : plugin_injection.resolved_dependency_hash_set())
    {
        if (!std::ranges::contains(out, dependency_hash))
        {
            out.push_back(dependency_hash);

            const auto dependency_iter = std::ranges::find(
                m_plugin_injections,
                dependency_hash,
                &internal::ErasedPluginInjection::hash
            );
            assert(dependency_iter != m_plugin_injections.cend());
            collect_all_resolved_dependency_hashes(*dependency_iter, out);
        }
    }
}

auto PluginTree::reestablish_internal_ordering_of_plugins(
    const internal::ErasedPluginInjection& new_plugin,
    std::pmr::memory_resource&             transitive_memory_resource
) -> void
{
    /*
     * Shift new injection and all its dependencies
     *  in front of the first dependent injection
     */


    auto first_dependent_injection_iter = std::ranges::find_if(
        m_plugin_injections,
        [new_plugin_hash =
             new_plugin.hash()](const internal::ErasedPluginInjection& injection) -> bool
        {
            return std::ranges::binary_search(
                injection.unresolved_dependency_hash_set(), new_plugin_hash
            );
        }
    );
    if (first_dependent_injection_iter == m_plugin_injections.cend())
    {
        return;
    }

    std::pmr::deque<uint64_t> all_resolved_dependency_hashes{
        std::initializer_list{ new_plugin.hash() }, &transitive_memory_resource
    };
    collect_all_resolved_dependency_hashes(new_plugin, all_resolved_dependency_hashes);
    std::ranges::sort(all_resolved_dependency_hashes);


    /*
     * Collect injections that need to be shifted in order
     */

    std::pmr::vector<decltype(first_dependent_injection_iter)>
        to_be_shifted_injection_iters{ &transitive_memory_resource };
    to_be_shifted_injection_iters.reserve(all_resolved_dependency_hashes.size());
    for (auto iter{ std::next(first_dependent_injection_iter) };   //
         iter != m_plugin_injections.cend();
         ++iter)
    {
        if (std::ranges::binary_search(all_resolved_dependency_hashes, iter->hash()))
        {
            to_be_shifted_injection_iters.push_back(iter);
        }
    }


    for (const auto& injection_iter : to_be_shifted_injection_iters)
    {
        /*
         * Shift injection in front of the first dependent injection
         */

        // TODO: use std::ranges::rotate when libc++ supports it
        first_dependent_injection_iter = std::rotate(
            first_dependent_injection_iter, injection_iter, std::next(injection_iter)
        );
    }
}

auto PluginTree::resolve(const uint64_t new_plugin_hash) -> void
{
    for (internal::ErasedPluginInjection& plugin_injection : m_plugin_injections)
    {
        plugin_injection.resolve_dependency(new_plugin_hash);
    }

    if (const auto iter = std::ranges::lower_bound(
            m_unresolved_optional_dependency_hashes, new_plugin_hash
        );
        iter != std::ranges::cend(m_unresolved_optional_dependency_hashes)
        && *iter == new_plugin_hash)
    {
        m_unresolved_optional_dependency_hashes.erase(iter);
    }
}

}   // namespace kiln::app
