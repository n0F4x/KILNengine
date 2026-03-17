module;

#include <algorithm>
#include <span>

module kiln.app.plugin.PluginInterface;

namespace kiln::app {

auto PluginInterface::configuration_dependency_hashes() const noexcept
    -> std::span<const uint64_t>
{
    return m_configuration_dependency_hashes;
}

auto PluginInterface::insert_configuration_dependency_hash(const uint64_t dependency_hash)
    -> void
{
    if (!std::ranges::binary_search(m_configuration_dependency_hashes, dependency_hash))
    {
        m_configuration_dependency_hashes.insert(
            std::ranges::upper_bound(m_configuration_dependency_hashes, dependency_hash),
            dependency_hash
        );
    }
}

}   // namespace kiln::app
