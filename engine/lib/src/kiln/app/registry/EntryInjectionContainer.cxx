module;

#include <algorithm>
#include <memory_resource>

module kiln.app.registry.EntryInjectionContainer;

namespace kiln::app {

EntryInjectionContainer::EntryInjectionContainer(
    EntryInjectionContainer&& other,
    const allocator_type&     allocator
)
    : m_injections{ std::move(other.m_injections), allocator },
      m_entry_hashes{ std::move(other.m_entry_hashes), allocator },
      m_dependency_entry_hashes{ std::move(other.m_dependency_entry_hashes), allocator }
{
}

EntryInjectionContainer::EntryInjectionContainer(const allocator_type& allocator)
    : m_injections{ allocator },
      m_entry_hashes{ allocator },
      m_dependency_entry_hashes{ allocator }
{
}

auto EntryInjectionContainer::get_allocator() const noexcept -> allocator_type
{
    return m_injections.get_allocator();
}

auto EntryInjectionContainer::sort(std::pmr::memory_resource& transient_memory_resource)
    -> void
{
    const std::pmr::vector<uint64_t> hashes{
        std::from_range,
        m_entry_hashes,
        &transient_memory_resource,
    };

    for (const uint64_t hash : hashes)
    {
        bubble_up_dependencies_of(hash, transient_memory_resource);
    }
}

auto EntryInjectionContainer::bubble_up_dependencies_of(
    const uint64_t             hash,
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    std::pmr::vector<uint64_t> all_dependencies{ &transient_memory_resource };
    collect_dependencies_of(hash, all_dependencies);

    auto hash_iter{ std::ranges::find(m_entry_hashes, hash) };
    auto injection_iter{
        std::next(
            m_injections.begin(),
            std::distance(m_entry_hashes.begin(), hash_iter)   //
        ),
    };
    auto dependencies_iter{
        std::next(
            m_dependency_entry_hashes.begin(),
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

        const auto dependency_injection_iter{
            std::next(
                m_injections.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)   //
            ),
        };
        const auto dependency_dependencies_iter{
            std::next(
                m_dependency_entry_hashes.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)   //
            ),
        };

        injection_iter = std::ranges::rotate(
                             injection_iter,
                             dependency_injection_iter,
                             std::next(dependency_injection_iter)
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

auto EntryInjectionContainer::collect_dependencies_of(
    const uint64_t              hash,
    std::pmr::vector<uint64_t>& result
) -> void
{
    [this, &result](this const auto& self, const std::size_t injection_index) -> void
    {
        for (const uint64_t dependency_hash : m_dependency_entry_hashes[injection_index])
        {
            if (const auto hash_iter{ std::ranges::find(m_entry_hashes, dependency_hash) };
                hash_iter != m_entry_hashes.cend())
            {
                if (const auto iter{ std::ranges::lower_bound(result, dependency_hash) };
                    iter != result.cend())
                {
                    result.insert(iter, dependency_hash);
                    self(
                        static_cast<std::size_t>(
                            std::distance(m_entry_hashes.begin(), hash_iter)
                        )
                    );
                }
            }
        }
    }(static_cast<std::size_t>(
        std::distance(m_entry_hashes.begin(), std::ranges::find(m_entry_hashes, hash))
    ));
}

}   // namespace kiln::app
