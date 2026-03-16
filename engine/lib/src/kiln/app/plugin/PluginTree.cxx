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
      m_plugin_type_hash{ std::move(other.m_plugin_type_hash) },
      m_plugin_name{ std::move(other.m_plugin_name) },
      m_unresolved_and_resolved_plugin_dependency_hashes{
          std::move(other.m_unresolved_and_resolved_plugin_dependency_hashes),
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

auto ErasedPluginInjection::plugin_type_hash() const noexcept -> uint64_t
{
    return m_plugin_type_hash;
}

auto ErasedPluginInjection::plugin_name() const noexcept -> std::string_view
{
    return m_plugin_name;
}

auto ErasedPluginInjection::plugin_dependency_hashes() const noexcept
    -> std::span<const uint64_t>
{
    return m_unresolved_and_resolved_plugin_dependency_hashes;
}

auto ErasedPluginInjection::resolved_plugin_dependency_hashes() const noexcept
    -> std::span<const uint64_t>
{
    return std::span{
        std::next(
            m_unresolved_and_resolved_plugin_dependency_hashes.cbegin(),
            static_cast<decltype(m_unresolved_and_resolved_plugin_dependency_hashes)::
                            difference_type>(m_unresolved_dependency_hash_count)
        ),
        m_unresolved_and_resolved_plugin_dependency_hashes.cend()
    };
}

auto ErasedPluginInjection::unresolved_plugin_dependency_hashes() const noexcept
    -> std::span<const uint64_t>
{
    return std::span{
        m_unresolved_and_resolved_plugin_dependency_hashes.cbegin(),
        std::next(
            m_unresolved_and_resolved_plugin_dependency_hashes.cbegin(),
            static_cast<decltype(m_unresolved_and_resolved_plugin_dependency_hashes)::
                            difference_type>(m_unresolved_dependency_hash_count)
        )
    };
}

auto ErasedPluginInjection::resolve_dependency(const uint64_t new_plugin_hash) -> void
{
    if (m_unresolved_dependency_hash_count == 0)
    {
        return;
    }

    const auto iter = std::ranges::find(
        m_unresolved_and_resolved_plugin_dependency_hashes, new_plugin_hash
    );
    if (iter == m_unresolved_and_resolved_plugin_dependency_hashes.cend())
    {
        return;
    }

    std::swap(
        *iter,
        m_unresolved_and_resolved_plugin_dependency_hashes
            [m_unresolved_dependency_hash_count - 1]
    );
    if (std::distance(m_unresolved_and_resolved_plugin_dependency_hashes.begin(), iter)
        < static_cast<decltype(m_unresolved_and_resolved_plugin_dependency_hashes)::
                          difference_type>(m_unresolved_dependency_hash_count))
    {
        m_unresolved_dependency_hash_count--;
    }
}

auto ErasedPluginInjection::operator()(PluginStack& plugin_stack) && -> void
{
    std::move(m_function)(plugin_stack);
}

}   // namespace internal

PluginTree::PluginTree(allocator_type allocator)
    : m_injections_resource{
          allocator.new_object<std::pmr::monotonic_buffer_resource>(allocator.resource()),
          util::Deleter{ allocator }
      },
      m_plugin_injections{ m_injections_resource.get() },
      m_unresolved_plugin_hashes_resource{
          allocator.new_object<std::pmr::unsynchronized_pool_resource>(
              allocator.resource()
          ),
          util::Deleter{ allocator }
      },
      m_unresolved_optional_dependency_plugin_hashes{
          m_unresolved_plugin_hashes_resource.get()
      }
{
}

auto PluginTree::invoke_plugins(App& app) && -> void
{
    internal::PluginStack plugin_stack;

    for (internal::ErasedPluginInjection& plugin_injection : m_plugin_injections)
    {
        std::move(plugin_injection)(plugin_stack);
    }

    std::move(plugin_stack)
        .for_each(
            [&app](ErasedPlugin&& erased_plugin) -> void
            {
                std::move(erased_plugin).configure_and_build(app);   //
            }
        );
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
    const uint64_t             dependency_plugin_hash,
    const PluginNameChainNode& visited_plugin_names
) const -> void
{
    PRECOND(
        new_plugin_hash != dependency_plugin_hash,
        std::format(
            "Cyclic dependency detected - plugin of type `{}` depends on itself "   //
            "({} -> {})",
            new_plugin_name,
            visited_plugin_names.format(),
            new_plugin_name
        )
    );

    const auto dependency_iter = std::ranges::find(
        m_plugin_injections,
        dependency_plugin_hash,
        &internal::ErasedPluginInjection::plugin_type_hash
    );
    if (dependency_iter == m_plugin_injections.cend())
    {
        return;
    }

    for (const uint64_t next_dependency_plugin_hash :
         dependency_iter->plugin_dependency_hashes())
    {
        const PluginNameChainNode plugin_chain_node{
            .previous    = &visited_plugin_names,
            .plugin_name = dependency_iter->plugin_name(),
        };

        check_for_new_cyclic_dependency(
            new_plugin_hash, new_plugin_name, next_dependency_plugin_hash, plugin_chain_node
        );
    }
}

auto PluginTree::collect_all_resolved_dependency_plugin_hashes(
    const uint64_t             plugin_hash,
    std::pmr::deque<uint64_t>& out
) const -> void
{
    const auto plugin_injection_iter = std::ranges::find(
        m_plugin_injections,
        plugin_hash,
        &internal::ErasedPluginInjection::plugin_type_hash
    );
    if (plugin_injection_iter == m_plugin_injections.cend())
    {
        return;
    }

    for (const uint64_t plugin_dependency_hash :
         plugin_injection_iter->resolved_plugin_dependency_hashes())
    {
        if (!std::ranges::contains(out, plugin_dependency_hash))
        {
            out.push_back(plugin_dependency_hash);
            collect_all_resolved_dependency_plugin_hashes(plugin_dependency_hash, out);
        }
    }
}

auto PluginTree::reestablish_internal_ordering_of_plugins(
    const uint64_t             new_plugin_hash,
    std::pmr::memory_resource& transitive_memory_resource
) -> void
{
    /*
     * Shift the new injection and all its dependencies in front of the first
     *  injection that depends on the new one
     */

    std::pmr::deque<uint64_t> all_dependency_plugin_hashes{
        std::initializer_list{ new_plugin_hash }, &transitive_memory_resource
    };
    collect_all_resolved_dependency_plugin_hashes(
        new_plugin_hash, all_dependency_plugin_hashes
    );

    auto first_dependent_injection_iter = std::ranges::find_if(
        m_plugin_injections,
        [new_plugin_hash](const internal::ErasedPluginInjection& injection) -> bool
        {
            return std::ranges::contains(
                injection.unresolved_plugin_dependency_hashes(), new_plugin_hash
            );
        }
    );
    if (first_dependent_injection_iter == m_plugin_injections.cend())
    {
        return;
    }

    for (const uint64_t dependency_plugin_hash : all_dependency_plugin_hashes)
    {
        /*
         * Shift injection in front of the first dependent injection
         */

        const auto injection_iter = std::ranges::find(
            std::ranges::subrange{ first_dependent_injection_iter,
                                   m_plugin_injections.cend() },
            dependency_plugin_hash,
            &internal::ErasedPluginInjection::plugin_type_hash
        );

        assert(
            injection_iter != first_dependent_injection_iter
            && "Cyclic dependency detected, but it should have been detected earlier"
        );

        if (injection_iter == m_plugin_injections.cend())
        {
            /*
             * Injection is already at the right place
             */
            continue;
        }

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

    m_unresolved_optional_dependency_plugin_hashes.remove(new_plugin_hash);
}

}   // namespace kiln::app
