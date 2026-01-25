#include "PluginTree.hpp"

#include <ranges>
#include <string>

namespace kiln::app {

namespace internal {

auto ErasedPluginInjection::plugin_type_hash() const noexcept -> uint64_t
{
    return m_plugin_type_hash;
}

auto ErasedPluginInjection::plugin_name() const noexcept -> std::string_view
{
    return m_plugin_name;
}

auto ErasedPluginInjection::resolved_plugin_dependency_hashes() const noexcept
    -> std::span<const uint64_t>
{
    return m_resolved_plugin_dependency_hashes;
}

auto ErasedPluginInjection::unresolved_plugin_dependency_hashes() const noexcept
    -> std::span<const uint64_t>
{
    return m_unresolved_plugin_dependency_hashes;
}

auto ErasedPluginInjection::resolve_dependency(const uint64_t new_plugin_hash) -> void
{
    if (std::erase(m_unresolved_plugin_dependency_hashes, new_plugin_hash) > 0)
    {
        m_resolved_plugin_dependency_hashes.push_back(new_plugin_hash);
    }
}

auto ErasedPluginInjection::operator()(PluginStack& plugin_stack) && -> void
{
    std::move(m_function)(plugin_stack);
}

}   // namespace internal

auto PluginTree::invoke_plugins(App& app) && -> void
{
    internal::PluginStack plugin_stack;

    for (internal::ErasedPluginInjection& plugin_injection : m_plugin_injections)
    {
        std::move(plugin_injection)(plugin_stack);
    }

    std::move(plugin_stack)
        .for_each(
            [&app](internal::ErasedPlugin&& erased_plugin) -> void
            {
                std::move(erased_plugin)(app);   //
            }
        );
}

auto PluginTree::check_for_new_cyclic_dependency(
    const uint64_t                 new_plugin_hash,
    const std::string_view         new_plugin_name,
    const uint64_t                 dependency_plugin_hash,
    const std::string_view         last_visited_plugin_name,
    std::vector<std::string_view>& visited_plugin_names
) const -> void
{
    PRECOND(
        new_plugin_hash != dependency_plugin_hash,
        std::format(
            "Cyclic dependency detected - plugin of type `{}` depends on itself "   //
            "({} -> {} -> {})",
            new_plugin_name,
            std::views::join_with(visited_plugin_names, " -> ")
                | std::ranges::to<std::string>(),
            last_visited_plugin_name,
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

    visited_plugin_names.push_back(last_visited_plugin_name);

    for (const uint64_t next_dependency_plugin_hash :
         dependency_iter->unresolved_plugin_dependency_hashes())
    {
        check_for_new_cyclic_dependency(
            new_plugin_hash,
            new_plugin_name,
            next_dependency_plugin_hash,
            dependency_iter->plugin_name(),
            visited_plugin_names
        );
    }

    visited_plugin_names.pop_back();
}

auto PluginTree::collect_all_resolved_dependency_plugin_hashes(
    const uint64_t         plugin_hash,
    std::vector<uint64_t>& out
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

auto PluginTree::reestablish_internal_ordering_of_plugins(const uint64_t new_plugin_hash)
    -> void
{
    /*
     * Shift the new injection and all its dependencies in front of the first
     *  injection that depends on the new one
     */

    std::vector all_dependency_plugin_hashes{ new_plugin_hash };
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

        if (injection_iter == m_plugin_injections.cend())
        {
            /*
             * Injection is already at the right place
             */
            continue;
        }

        first_dependent_injection_iter =
            std::ranges::rotate(
                first_dependent_injection_iter, injection_iter, std::next(injection_iter)
            )
                .next()
                .begin();
    }
}

auto PluginTree::resolve(const uint64_t new_plugin_hash) -> void
{
    for (internal::ErasedPluginInjection& plugin_injection : m_plugin_injections)
    {
        plugin_injection.resolve_dependency(new_plugin_hash);
    }

    std::erase(m_unresolved_optional_dependency_plugin_types, new_plugin_hash);
}

}   // namespace kiln::app
