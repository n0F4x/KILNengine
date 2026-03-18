module;

#include <algorithm>
#include <iterator>
#include <span>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.app.plugin.PluginInterface;

import kiln.util.contracts;

namespace kiln::app {

auto PluginInterface::configuration_dependency_hash_set() const noexcept
    -> std::span<const uint64_t>
{
    return m_configuration_dependency_hash_set;
}

auto PluginInterface::dependency_hash_set() const noexcept -> std::span<const uint64_t>
{
    return m_dependency_hash_set;
}

auto PluginInterface::set_resolved_dependency_hash_set(
    const std::span<const uint64_t> dependency_hash_set
) -> void
{
    PRECOND(
        m_dependency_hash_set.empty()
        && "`set_resolved_dependency_hash_set` should get called before any configuration is registered"
    );

    m_dependency_hash_set.append_range(dependency_hash_set);
}

auto PluginInterface::insert_configuration_dependency_hash(const uint64_t dependency_hash)
    -> void
{
    if (const auto config_iter{ std::ranges::lower_bound(
            m_configuration_dependency_hash_set, dependency_hash
        ) };
        config_iter == m_configuration_dependency_hash_set.cend()
        || *config_iter != dependency_hash)
    {
        m_configuration_dependency_hash_set.insert(config_iter, dependency_hash);

        if (const auto general_iter{
                std::ranges::lower_bound(m_dependency_hash_set, dependency_hash) };
            general_iter == m_dependency_hash_set.cend()
            || *general_iter != dependency_hash)
        {
            m_dependency_hash_set.insert(general_iter, dependency_hash);
        }
    }
}

}   // namespace kiln::app
