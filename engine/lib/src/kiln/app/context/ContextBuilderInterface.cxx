module;

#include <algorithm>
#include <iterator>
#include <span>
#include <vector>

module kiln.app.context.ContextBuilderInterface;

namespace kiln::app {

auto configuration_dependency_hash_set(const ContextBuilderInterface& builder) noexcept
    -> std::span<const uint64_t>
{
    return builder.m_configuration_dependency_hash_set;
}

auto dependency_hashes(const ContextBuilderInterface& builder) noexcept
    -> std::span<const uint64_t>
{
    return builder.m_dependency_hashes;
}

auto set_resolved_dependency_hashes(
    ContextBuilderInterface&        builder,
    const std::span<const uint64_t> resolved_dependency_hashes
) -> void
{
    builder.m_dependency_hashes.append_range(resolved_dependency_hashes);
}

auto ContextBuilderInterface::insert_configuration_dependency_hash(
    const uint64_t dependency_hash
) -> void
{
    if (const auto config_iter{ std::ranges::lower_bound(
            m_configuration_dependency_hash_set,
            dependency_hash
        ) };
        config_iter == m_configuration_dependency_hash_set.cend()
        || *config_iter != dependency_hash)
    {
        m_configuration_dependency_hash_set.insert(config_iter, dependency_hash);

        if (const auto general_iter{
                std::ranges::lower_bound(m_dependency_hashes, dependency_hash) };
            general_iter == m_dependency_hashes.cend()
            || *general_iter != dependency_hash)
        {
            m_dependency_hashes.insert(general_iter, dependency_hash);
        }
    }
}

}   // namespace kiln::app
