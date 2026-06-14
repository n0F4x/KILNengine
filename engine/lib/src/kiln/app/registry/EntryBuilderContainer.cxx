module;

#include <algorithm>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <span>
#include <vector>

module kiln.app.registry.EntryBuilderContainer;

namespace kiln::app {

EntryBuilderContainer::EntryBuilderContainer(
    EntryBuilderContainer&& other,
    const allocator_type&   allocator
)
    : m_builders{ std::move(other.m_builders), allocator },
      m_entry_hashes{ std::move(other.m_entry_hashes), allocator },
      m_builder_dependency_hashes{
          std::move(other.m_builder_dependency_hashes),
          allocator,
      },
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
    sort_based_on_entry_dependencies(transient_memory_resource);
    sort_based_on_builder_dependencies(transient_memory_resource);
}

auto EntryBuilderContainer::build(
    Registry&                  registry,
    std::pmr::memory_resource& transient_memory_resource
) && -> void
{
    sort(transient_memory_resource);

    // TODO: check for cyclic dependencies

    for (ErasedEntryBuilder& builder : m_builders)
    {
        std::move(builder)(*this, registry);
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

auto EntryBuilderContainer::sort_based_on_builder_dependencies(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    std::pmr::vector<std::pmr::vector<uint64_t>> dependent_hashes_collection{
        &transient_memory_resource
    };
    dependent_hashes_collection.reserve(m_builders.size());
    collect_dependent_builder_hashes(dependent_hashes_collection);

    bool                   done_sorting{};
    std::pmr::vector<bool> done_sorting_based_on_builder(
        m_builders.size(),
        false,
        &transient_memory_resource
    );
    while (!done_sorting)
    {
        done_sorting = true;
        for (auto&& [index, hash, dependent_hashes] : std::views::zip(
                 std::views::iota(0uz),
                 m_entry_hashes,
                 dependent_hashes_collection
             ))
        {
            if (done_sorting_based_on_builder[index])
            {
                continue;
            }

            if (dependent_hashes.empty())
            {
                for (auto& dependent_dependency_hashes : dependent_hashes_collection)
                {
                    std::erase(dependent_dependency_hashes, hash);
                }

                /*
                 * Pushing down invalidates iterators
                 */
                push_down_builder_dependencies_of(index);
            }
            else
            {
                done_sorting = false;
            }
        }
    }
}

auto EntryBuilderContainer::sort_based_on_entry_dependencies(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    std::pmr::vector<std::pmr::vector<uint64_t>> dependent_hashes_collection{
        &transient_memory_resource
    };
    dependent_hashes_collection.reserve(m_builders.size());
    collect_dependent_entry_hashes(dependent_hashes_collection);

    bool                   done_sorting{};
    std::pmr::vector<bool> done_sorting_based_on_builder(
        m_builders.size(),
        false,
        &transient_memory_resource
    );
    while (!done_sorting)
    {
        done_sorting = true;
        for (auto&& [index, hash, dependent_hashes] : std::views::zip(
                 std::views::iota(0uz),
                 m_entry_hashes,
                 dependent_hashes_collection
             ))
        {
            if (done_sorting_based_on_builder[index])
            {
                continue;
            }

            if (dependent_hashes.empty())
            {
                for (auto& dependent_dependency_hashes : dependent_hashes_collection)
                {
                    std::erase(dependent_dependency_hashes, hash);
                }

                /*
                 * Pushing down invalidates iterators
                 */
                bubble_up_entry_dependencies_of(index);
            }
            else
            {
                done_sorting = false;
            }
        }
    }
}

auto EntryBuilderContainer::collect_dependent_builder_hashes(
    std::pmr::vector<std::pmr::vector<uint64_t>>& out
) const -> void
{
    for (const uint64_t hash : m_entry_hashes)
    {
        auto& dependent_builder_hashes{ out.emplace_back() };
        collect_dependent_builder_hashes_of(hash, dependent_builder_hashes);
    }
}

auto EntryBuilderContainer::collect_dependent_builder_hashes_of(
    const uint64_t              hash,
    std::pmr::vector<uint64_t>& out
) const -> void
{
    for (const auto& [dependent_hash, builder_dependency_hashes] :
         std::views::zip(m_entry_hashes, m_builder_dependency_hashes))
    {
        if (std::ranges::contains(builder_dependency_hashes, hash))
        {
            out.push_back(dependent_hash);
        }
    }
}

auto EntryBuilderContainer::collect_dependent_entry_hashes(
    std::pmr::vector<std::pmr::vector<uint64_t>>& out
) const -> void
{
    for (const uint64_t hash : m_entry_hashes)
    {
        auto& dependent_entry_hashes{ out.emplace_back() };
        collect_dependent_entry_hashes_of(hash, dependent_entry_hashes);
    }
}

auto EntryBuilderContainer::collect_dependent_entry_hashes_of(
    const uint64_t              hash,
    std::pmr::vector<uint64_t>& out
) const -> void
{
    for (const auto& [dependent_hash, entry_dependency_hashes] :
         std::views::zip(m_entry_hashes, m_entry_dependency_hashes))
    {
        if (std::ranges::contains(entry_dependency_hashes, hash))
        {
            out.push_back(dependent_hash);
        }
    }
}

auto EntryBuilderContainer::push_down_builder_dependencies_of(const std::size_t index)
    -> void
{
    const auto zipped_containers{
        std::views::zip(
            m_builders,
            m_entry_hashes,
            m_builder_dependency_hashes,
            m_entry_dependency_hashes
        ),
    };

    auto iter{
        std::next(
            zipped_containers.begin(),
            static_cast<std::iter_difference_t<decltype(zipped_containers.begin())>>(index)
        ),
    };

    for (const std::span dependency_hashes{ m_builder_dependency_hashes[index] };
         const uint64_t  dependency_hash : dependency_hashes)
    {
        const auto hash_iter{
            std::next(
                m_entry_hashes.begin(),
                std::distance(zipped_containers.begin(), iter)
            ),
        };
        const auto dependency_hash_iter{
            std::ranges::find(m_entry_hashes.begin(), hash_iter, dependency_hash),
        };
        if (dependency_hash_iter == hash_iter)
        {
            continue;
        }

        const auto dependency_iter{
            std::next(
                zipped_containers.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)
            ),
        };

        iter = std::prev(
            std::ranges::rotate(
                dependency_iter,
                std::next(dependency_iter),
                std::next(iter)
            )
                .begin()
        );
    }
}

auto EntryBuilderContainer::bubble_up_entry_dependencies_of(const std::size_t index)
    -> void
{
    const auto zipped_containers{
        std::views::zip(
            m_builders,
            m_entry_hashes,
            m_builder_dependency_hashes,
            m_entry_dependency_hashes
        ),
    };

    auto iter{
        std::next(
            zipped_containers.begin(),
            static_cast<std::iter_difference_t<decltype(zipped_containers.begin())>>(index)
        ),
    };

    for (const std::span dependency_hashes{ m_entry_dependency_hashes[index] };
         const uint64_t  dependency_hash : dependency_hashes)
    {
        const auto dependency_hash_iter{
            std::ranges::find(
                std::next(
                    m_entry_hashes.begin(),
                    std::distance(zipped_containers.begin(), std::next(iter))
                ),
                m_entry_hashes.end(),
                dependency_hash
            ),
        };
        if (dependency_hash_iter == m_entry_hashes.cend())
        {
            continue;
        }

        const auto dependency_iter{
            std::next(
                zipped_containers.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)
            ),
        };

        iter = std::ranges::rotate(iter, dependency_iter, std::next(dependency_iter))
                   .begin();
    }
}

}   // namespace kiln::app
