module;

#include <algorithm>
#include <memory_resource>
#include <optional>
#include <ranges>

module kiln.app.registry.EntryBuilderContainer;

namespace kiln::app {

EntryBuilderContainer::EntryBuilderContainer(
    EntryBuilderContainer&& other,
    const allocator_type&   allocator
)
    : m_builders{ std::move(other.m_builders), allocator },
      m_entry_hashes{ std::move(other.m_entry_hashes), allocator },
      m_builder_dependency_hashes{ std::move(other.m_builder_dependency_hashes),
                                   allocator },
      m_entry_dependency_hashes{ std::move(other.m_entry_dependency_hashes), allocator }
{
}

EntryBuilderContainer::EntryBuilderContainer(const allocator_type& allocator)
    : m_builders{ allocator },
      m_entry_hashes{ allocator },
      m_builder_dependency_hashes{ allocator },
      m_entry_dependency_hashes{ allocator }
{
}

auto EntryBuilderContainer::get_allocator() const noexcept -> allocator_type
{
    return m_builders.get_allocator();
}

auto EntryBuilderContainer::sort(std::pmr::memory_resource& transient_memory_resource)
    -> void
{
    const std::pmr::vector<uint64_t> hashes{
        std::from_range,
        m_entry_hashes,
        &transient_memory_resource,
    };

    for (const uint64_t hash : hashes)
    {
        bubble_up_entry_dependencies_of(hash, transient_memory_resource);
    }
    for (const uint64_t hash : hashes)
    {
        push_down_builder_dependencies_of(hash, transient_memory_resource);
    }
}

auto EntryBuilderContainer::try_index_of_builder(const uint64_t hash) const noexcept
    -> std::optional<std::size_t>
{
    const auto hash_iter{ std::ranges::find(m_entry_hashes, hash) };
    if (hash_iter == m_entry_hashes.cend())
    {
        return std::nullopt;
    }
    return static_cast<std::size_t>(std::distance(m_entry_hashes.begin(), hash_iter));
}

auto EntryBuilderContainer::bubble_up_entry_dependencies_of(
    const uint64_t             hash,
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    std::pmr::vector<uint64_t> all_dependencies{ &transient_memory_resource };
    collect_entry_dependencies_of(hash, all_dependencies);

    auto hash_iter{ std::ranges::find(m_entry_hashes, hash) };
    auto builder_iter{
        std::next(
            m_builders.begin(),
            std::distance(m_entry_hashes.begin(), hash_iter)   //
        ),
    };
    auto dependencies_iter{
        std::next(
            m_entry_dependency_hashes.begin(),
            std::distance(m_entry_hashes.begin(), hash_iter)   //
        ),
    };

    for (const uint64_t dependency_hash : all_dependencies)
    {
        const auto dependency_hash_iter{
            std::ranges::find(std::next(hash_iter), m_entry_hashes.end(), dependency_hash),
        };
        if (dependency_hash_iter == m_entry_hashes.cend())
        {
            continue;
        }

        const auto dependency_builder_iter{
            std::next(
                m_builders.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)   //
            ),
        };
        const auto dependency_dependencies_iter{
            std::next(
                m_entry_dependency_hashes.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)   //
            ),
        };

        builder_iter = std::ranges::rotate(
                           builder_iter,
                           dependency_builder_iter,
                           std::next(dependency_builder_iter)
        )
                           .begin();
        hash_iter = std::ranges::rotate(
                        hash_iter,
                        dependency_hash_iter,
                        std::next(dependency_hash_iter)
        )
                        .begin();
        dependencies_iter = std::ranges::rotate(
                                dependencies_iter,
                                dependency_dependencies_iter,
                                std::next(dependency_dependencies_iter)
        )
                                .begin();
    }
}

auto EntryBuilderContainer::push_down_builder_dependencies_of(
    const uint64_t             hash,
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    std::pmr::vector<uint64_t> all_dependencies{ &transient_memory_resource };
    collect_builder_dependencies_of(hash, all_dependencies);

    auto hash_iter{ std::ranges::find(m_entry_hashes, hash) };
    auto builder_iter{
        std::next(
            m_builders.begin(),
            std::distance(m_entry_hashes.begin(), hash_iter)   //
        ),
    };
    auto dependencies_iter{
        std::next(
            m_entry_dependency_hashes.begin(),
            std::distance(m_entry_hashes.begin(), hash_iter)   //
        ),
    };

    for (const uint64_t dependency_hash : all_dependencies)
    {
        const auto dependency_hash_iter{
            std::ranges::find(m_entry_hashes.begin(), hash_iter, dependency_hash),
        };
        if (dependency_hash_iter == hash_iter)
        {
            continue;
        }

        const auto dependency_builder_iter{
            std::next(
                m_builders.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)   //
            ),
        };
        const auto dependency_dependencies_iter{
            std::next(
                m_entry_dependency_hashes.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)   //
            ),
        };

        builder_iter = std::prev(
            std::ranges::rotate(
                dependency_builder_iter,
                std::next(dependency_builder_iter),
                std::next(builder_iter)
            )
                .begin()
        );
        hash_iter = std::prev(
            std::ranges::rotate(
                dependency_hash_iter,
                std::next(dependency_hash_iter),
                std::next(hash_iter)
            )
                .begin()
        );
        dependencies_iter = std::prev(
            std::ranges::rotate(
                dependency_dependencies_iter,
                std::next(dependency_dependencies_iter),
                std::next(dependencies_iter)
            )
                .begin()
        );
    }
}

auto EntryBuilderContainer::collect_entry_dependencies_of(
    const uint64_t              hash,
    std::pmr::vector<uint64_t>& result
) const -> void
{
    [this, &result](this const auto& self, const std::size_t builder_index) -> void
    {
        for (const uint64_t dependency_hash : m_entry_dependency_hashes[builder_index])
        {
            if (const std::optional<std::size_t> dependency_index{
                    try_index_of_builder(dependency_hash) };
                dependency_index.has_value())
            {
                if (const auto iter{ std::ranges::lower_bound(result, dependency_hash) };
                    iter != result.cend())
                {
                    result.insert(iter, dependency_hash);
                    self(*dependency_index);
                }
            }
        }
    }(*try_index_of_builder(hash));
}

auto EntryBuilderContainer::collect_builder_dependencies_of(
    const uint64_t              hash,
    std::pmr::vector<uint64_t>& result
) const -> void
{
    [this, &result](this const auto& self, const std::size_t builder_index) -> void
    {
        for (const uint64_t dependency_hash : m_builder_dependency_hashes[builder_index])
        {
            if (const std::optional<std::size_t> dependency_index{
                    try_index_of_builder(dependency_hash) };
                dependency_index.has_value())
            {
                if (const auto iter{ std::ranges::lower_bound(result, dependency_hash) };
                    iter != result.cend())
                {
                    result.insert(iter, dependency_hash);
                    self(*dependency_index);
                }
            }
        }
    }(*try_index_of_builder(hash));
}

}   // namespace kiln::app
