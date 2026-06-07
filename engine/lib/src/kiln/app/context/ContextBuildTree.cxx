module;

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>
#include <format>
#include <ranges>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.app.context.ContextBuildTree;

import kiln.app.config.ConfigBuilder;
import kiln.app.context.ErasedContextBuilder;
import kiln.app.memory.MemoryArenaBuilder;
import kiln.util.contracts;

namespace kiln::app {

ContextBuildTree::ContextBuildTree(MemoryArena&& memory_arena, const Config& config)
    : m_contexts{ memory_arena.pool_allocator() },
      m_builders{ memory_arena.pool_allocator() },
      m_builder_hashes{ memory_arena.pool_allocator() },
      m_builder_dependencies{ memory_arena.pool_allocator() },
      m_injections{ memory_arena.pool_allocator() },
      m_injection_hashes{ memory_arena.pool_allocator() },
      m_injection_dependencies{ memory_arena.pool_allocator() },
      m_contained_hashes{ memory_arena.pool_allocator() }
{
    unsafe_register_context_builder(MemoryArenaBuilder{ std::move(memory_arena) });
    unsafe_register_context_builder(ConfigBuilder{ config });
}

auto ContextBuildTree::get_allocator() const noexcept -> allocator_type
{
    return m_contexts.get_allocator();
}

auto ContextBuildTree::build(
    std::pmr::memory_resource& transient_memory_resource
) && -> Contexts
{
    for (Injector injector{ *this }; ErasedInjection& injection : m_injections)
    {
        std::move(injection)(injector, transient_memory_resource);
    }

    for (auto&& [dependency_descriptor, builder] :
         std::views::zip(m_builder_dependencies, m_builders))
    {
        set_resolved_dependency_hashes(
            *builder,
            dependency_descriptor.resolved_dependencies()
        );
    }
    PRECOND(!check_for_configuration_dependencies());
    PRECOND(!check_for_cyclic_configuration_dependencies());
    fix_build_order(transient_memory_resource);

    for (ErasedContextBuilder& builder : m_builders)
    {
        std::move(builder).configure_and_build(m_contexts);
    }

    return std::move(m_contexts);
}

ContextBuildTree::Injector::Injector(ContextBuildTree& context_build_tree)
    : m_build_tree{ context_build_tree }
{
}

auto ContextBuildTree::DependencyChainNode::contains(const uint64_t hash) const noexcept
    -> bool
{
    return this->hash == hash || (previous != nullptr && previous->contains(hash));
}

auto ContextBuildTree::DependencyChainNode::format(
    const std::pmr::string::allocator_type& allocator
) const -> std::pmr::string
{
    std::pmr::string result{ allocator };
    format(result, 0);
    return result;
}

auto ContextBuildTree::DependencyChainNode::format(
    std::pmr::string& out,
    const std::size_t capacity
) const -> void
{
    if (previous != nullptr)
    {
        previous->format(out, capacity + std::strlen(" -> ") + name.length());
        out.append(" -> ");
    }
    else
    {
        assert(
            out.empty() && "this invocation should be the one to start building the string"
        );
        out.reserve(capacity + name.size());
    }

    out.append(std::format("{}", name));
}

ContextBuildTree::DependencyDescriptor::DependencyDescriptor(
    const DependencyDescriptor& other,
    const allocator_type&       allocator
)
    : m_hash{ other.m_hash },
      m_all_missing_and_resolved_dependency_hashes{
          other.m_all_missing_and_resolved_dependency_hashes,
          allocator
      },
      m_number_of_missing_dependencies{ other.m_number_of_missing_dependencies }
{
}

ContextBuildTree::DependencyDescriptor::DependencyDescriptor(
    DependencyDescriptor&& other,
    const allocator_type&  allocator
)
    : m_hash{ other.m_hash },
      m_all_missing_and_resolved_dependency_hashes{
          std::move(other.m_all_missing_and_resolved_dependency_hashes),
          allocator,
      },
      m_number_of_missing_dependencies{ other.m_number_of_missing_dependencies }
{
}

ContextBuildTree::DependencyDescriptor::DependencyDescriptor(
    std::allocator_arg_t,
    const allocator_type& allocator,
    const uint64_t        hash,
    std::pair<std::pmr::deque<uint64_t>, uint32_t>&&
        all_missing_and_resolved_dependency_hashes
)
    : m_hash{ hash },
      m_all_missing_and_resolved_dependency_hashes{
          std::from_range,
          std::move(all_missing_and_resolved_dependency_hashes.first),
          allocator,
      },
      m_number_of_missing_dependencies{ all_missing_and_resolved_dependency_hashes.second }
{
}

auto ContextBuildTree::DependencyDescriptor::get_allocator() const noexcept
    -> allocator_type
{
    return m_all_missing_and_resolved_dependency_hashes.get_allocator();
}

auto ContextBuildTree::DependencyDescriptor::hash() const noexcept -> uint64_t
{
    return m_hash;
}

auto ContextBuildTree::DependencyDescriptor::misses_dependency(
    const uint64_t hash
) const noexcept -> bool
{
    return std::ranges::binary_search(
        m_all_missing_and_resolved_dependency_hashes.cbegin(),
        std::next(
            m_all_missing_and_resolved_dependency_hashes.cbegin(),
            m_number_of_missing_dependencies
        ),
        hash
    );
}

auto ContextBuildTree::DependencyDescriptor::resolved_dependencies() const noexcept
    -> std::span<const uint64_t>
{
    return std::views::drop(
        m_all_missing_and_resolved_dependency_hashes,
        m_number_of_missing_dependencies
    );
}

auto ContextBuildTree::DependencyDescriptor::resolve_missing_dependency(
    const uint64_t hash
) -> void
{
    const auto missing_hashes{
        std::views::take(
            m_all_missing_and_resolved_dependency_hashes,
            m_number_of_missing_dependencies
        )   //
    };

    const auto iter = std::ranges::find(missing_hashes, hash);
    if (iter == std::ranges::cend(missing_hashes))
    {
        return;
    }

    std::ranges::rotate(iter, std::next(iter), missing_hashes.end());
    --m_number_of_missing_dependencies;
}

auto ContextBuildTree::context_builder(const uint64_t hash) const
    -> const ErasedContextBuilder&
{
    return *find_context_builder(hash);
}

auto ContextBuildTree::find_context_builder(const uint64_t hash)
    -> util::OptionalRef<ErasedContextBuilder>
{
    const auto builder_hash_iter{ std::ranges::find(m_builder_hashes, hash) };
    if (builder_hash_iter == m_builder_hashes.cend() || *builder_hash_iter != hash)
    {
        return std::nullopt;
    }

    return *std::next(
        m_builders.begin(),
        std::distance(m_builder_hashes.begin(), builder_hash_iter)
    );
}

auto ContextBuildTree::find_context_builder(const uint64_t hash) const
    -> util::OptionalRef<const ErasedContextBuilder>
{
    const auto builder_hash_iter{ std::ranges::find(m_builder_hashes, hash) };
    if (builder_hash_iter == m_builder_hashes.cend() || *builder_hash_iter != hash)
    {
        return std::nullopt;
    }

    return *std::next(
        m_builders.cbegin(),
        std::distance(m_builder_hashes.begin(), builder_hash_iter)
    );
}

auto ContextBuildTree::fix_order_of_builders(
    const uint64_t                  latest_hash,
    const std::span<const uint64_t> resolved_builder_dependencies,
    std::pmr::memory_resource&      transient_memory_resource
) -> void
{
    /*
     * Shift new builder and all its dependencies
     *  in front of the first dependent builder
     */


    auto first_dependent_builder_descriptor_iter = std::ranges::find_if(
        m_builder_dependencies,
        [latest_hash](const DependencyDescriptor& descriptor) -> bool
        {
            return descriptor.misses_dependency(latest_hash);   //
        }
    );
    if (first_dependent_builder_descriptor_iter == m_builder_dependencies.cend())
    {
        return;
    }
    auto first_dependent_builder_iter{
        std::next(
            m_builders.begin(),
            std::distance(
                m_builder_dependencies.begin(),
                first_dependent_builder_descriptor_iter
            )
        )   //
    };
    auto first_dependent_builder_hash_iter{
        std::next(
            m_builder_hashes.begin(),
            std::distance(
                m_builder_dependencies.begin(),
                first_dependent_builder_descriptor_iter
            )
        )   //
    };


    /*
     * Collect builders that need to be shifted in order
     */

    std::pmr::vector<decltype(first_dependent_builder_descriptor_iter)>
        to_be_shifted_builder_descriptor_iters{ &transient_memory_resource };
    to_be_shifted_builder_descriptor_iters.reserve(
        resolved_builder_dependencies.size() + 1
    );
    for (auto iter{ std::next(first_dependent_builder_descriptor_iter) };   //
         iter != m_builder_dependencies.cend();
         ++iter)
    {
        if (iter->hash() == latest_hash
            || std::ranges::binary_search(resolved_builder_dependencies, iter->hash()))
        {
            to_be_shifted_builder_descriptor_iters.push_back(iter);
        }
    }


    for (const auto& builder_descriptor_iter : to_be_shifted_builder_descriptor_iters)
    {
        /*
         * Shift builder in front of the first dependent builder
         */

        const auto index{
            std::distance(m_builder_dependencies.begin(), builder_descriptor_iter)
        };

        first_dependent_builder_descriptor_iter
            = std::ranges::rotate(
                  first_dependent_builder_descriptor_iter,
                  builder_descriptor_iter,
                  std::next(builder_descriptor_iter)
            )
                  .begin();
        first_dependent_builder_iter = std::ranges::rotate(
                                           first_dependent_builder_iter,
                                           std::next(m_builders.begin(), index),
                                           std::next(m_builders.begin(), index + 1)
        )
                                           .begin();
        first_dependent_builder_hash_iter
            = std::ranges::rotate(
                  first_dependent_builder_hash_iter,
                  std::next(m_builder_hashes.begin(), index),
                  std::next(m_builder_hashes.begin(), index + 1)
            )
                  .begin();
    }
}

auto ContextBuildTree::mark_builder_as_resolved(const uint64_t hash) -> void
{
    for (DependencyDescriptor& descriptor : m_builder_dependencies)
    {
        descriptor.resolve_missing_dependency(hash);
    }
}

auto ContextBuildTree::fix_order_of_injections(
    const uint64_t                  latest_hash,
    const std::span<const uint64_t> resolved_injection_dependencies,
    std::pmr::memory_resource&      transient_memory_resource
) -> void
{
    /*
     * Shift new injection and all its dependencies
     *  in front of the first dependent injection
     */


    auto first_dependent_injection_descriptor_iter = std::ranges::find_if(
        m_injection_dependencies,
        [latest_hash](const DependencyDescriptor& descriptor) -> bool
        {
            return descriptor.misses_dependency(latest_hash);   //
        }
    );
    if (first_dependent_injection_descriptor_iter == m_injection_dependencies.cend())
    {
        return;
    }
    auto first_dependent_injection_iter{
        std::next(
            m_injections.begin(),
            std::distance(
                m_injection_dependencies.begin(),
                first_dependent_injection_descriptor_iter
            )
        )   //
    };
    auto first_dependent_injection_hash_iter{
        std::next(
            m_injection_hashes.begin(),
            std::distance(
                m_injection_dependencies.begin(),
                first_dependent_injection_descriptor_iter
            )
        )   //
    };


    /*
     * Collect injections that need to be shifted in order
     */

    std::pmr::vector<decltype(first_dependent_injection_descriptor_iter)>
        to_be_shifted_injection_descriptor_iters{ &transient_memory_resource };
    to_be_shifted_injection_descriptor_iters.reserve(
        resolved_injection_dependencies.size() + 1
    );
    for (auto iter{ std::next(first_dependent_injection_descriptor_iter) };   //
         iter != m_injection_dependencies.cend();
         ++iter)
    {
        if (iter->hash() == latest_hash
            || std::ranges::binary_search(resolved_injection_dependencies, iter->hash()))
        {
            to_be_shifted_injection_descriptor_iters.push_back(iter);
        }
    }


    for (const auto& injection_descriptor_iter : to_be_shifted_injection_descriptor_iters)
    {
        /*
         * Shift injection in front of the first dependent injection
         */

        const auto index{
            std::distance(m_injection_dependencies.begin(), injection_descriptor_iter)
        };

        first_dependent_injection_descriptor_iter
            = std::ranges::rotate(
                  first_dependent_injection_descriptor_iter,
                  injection_descriptor_iter,
                  std::next(injection_descriptor_iter)
            )
                  .begin();
        first_dependent_injection_iter = std::ranges::rotate(
                                             first_dependent_injection_iter,
                                             std::next(m_injections.begin(), index),
                                             std::next(m_injections.begin(), index + 1)
        )
                                             .begin();
        first_dependent_injection_hash_iter
            = std::ranges::rotate(
                  first_dependent_injection_hash_iter,
                  std::next(m_injection_hashes.begin(), index),
                  std::next(m_injection_hashes.begin(), index + 1)
            )
                  .begin();
    }
}

auto ContextBuildTree::mark_injection_as_resolved(const uint64_t hash) -> void
{
    for (DependencyDescriptor& descriptor : m_injection_dependencies)
    {
        descriptor.resolve_missing_dependency(hash);
    }
}

auto ContextBuildTree::check_for_configuration_dependencies() const -> bool
{
    return std::ranges::any_of(
        m_builders,
        [this](const ErasedContextBuilder& builder) -> bool
        {
            return check_for_configuration_dependencies(builder);   //
        }
    );
}

auto ContextBuildTree::check_for_configuration_dependencies(
    const ErasedContextBuilder& erased_builder
) const -> bool
{
    return std::ranges::any_of(
        configuration_dependency_hash_set(*erased_builder),
        [&](const uint64_t configuration_dependency_hash) -> bool
        {
            if (!contains(configuration_dependency_hash))
            {
                PRECOND(
                    false,
                    std::format(
                        "Missing configuration dependency for builder `{}`",
                        erased_builder.name()
                    )
                );
                return true;
            }
            return false;
        }
    );
}

auto ContextBuildTree::check_for_cyclic_configuration_dependencies() const -> bool
{
    return std::ranges::any_of(
        m_builders,
        [this](const ErasedContextBuilder& builder) -> bool
        {
            return check_for_cyclic_configuration_dependencies(builder);   //
        }
    );
}

auto ContextBuildTree::check_for_cyclic_configuration_dependencies(
    const ErasedContextBuilder& erased_builder
) const -> bool
{
    const DependencyChainNode builder_name_chain_node{
        .name = erased_builder.name(),
    };

    return std::ranges::any_of(
        configuration_dependency_hash_set(*erased_builder),
        [&](const uint64_t configuration_dependency_hash) -> bool
        {
            return check_for_cyclic_configuration_dependency(
                erased_builder.hash(),
                erased_builder.name(),
                context_builder(configuration_dependency_hash).name(),
                configuration_dependency_hash,
                builder_name_chain_node
            );
        }
    );
}

auto ContextBuildTree::check_for_cyclic_configuration_dependency(
    const uint64_t             hash,
    const std::string_view     original_builder_name,
    const std::string_view     original_dependency_name,
    const uint64_t             dependency_hash,
    const DependencyChainNode& visited_dependency_names
) const -> bool
{
    if (hash == dependency_hash)
    {
        PRECOND(
            false,
            std::format(
                "Cyclic dependency detected through configuration:"   //
                " See `{}` being configured using `{}`"               //
                " ({} -> {})",
                original_builder_name,
                original_dependency_name,
                visited_dependency_names.format(),
                original_builder_name
            )
        );
        return true;
    }

    const util::OptionalRef<const ErasedContextBuilder> dependency_builder{
        find_context_builder(dependency_hash)
    };
    if (!dependency_builder.has_value())
    {
        return false;
    }

    return std::ranges::any_of(
        dependency_hashes(**dependency_builder),
        [&](const uint64_t next_dependency_hash) -> bool
        {
            const DependencyChainNode dependency_chain_node{
                .previous = &visited_dependency_names,
                .name     = dependency_builder->name(),
            };

            return check_for_cyclic_configuration_dependency(
                hash,
                original_builder_name,
                original_dependency_name,
                next_dependency_hash,
                dependency_chain_node
            );
        }
    );
}

auto ContextBuildTree::collect_all_resolved_build_dependency_hashes(
    const uint64_t                                   hash,
    const std::pmr::polymorphic_allocator<uint64_t>& allocator
) -> std::pmr::deque<uint64_t>
{
    std::pmr::deque<uint64_t> result{
        std::initializer_list{ hash },
        allocator,
    };

    collect_all_resolved_build_dependency_hashes(context_builder(hash), result);

    return result;
}

auto ContextBuildTree::collect_all_resolved_build_dependency_hashes(
    const ErasedContextBuilder& builder,
    std::pmr::deque<uint64_t>&  out
) -> void
{
    for (const uint64_t dependency_hash : dependency_hashes(*builder))
    {
        if (const util::OptionalRef dependency{ find_context_builder(dependency_hash) };
            dependency.has_value() && !std::ranges::binary_search(out, dependency_hash))
        {
            out.insert(std::ranges::upper_bound(out, dependency_hash), dependency_hash);
            collect_all_resolved_build_dependency_hashes(*dependency, out);
        }
    }
}

auto ContextBuildTree::fix_build_order(
    std::pmr::memory_resource& transient_memory_resource
) -> void
{
    std::pmr::vector<std::pair<uint64_t, std::span<const uint64_t>>>
        build_hashes_and_dependencies{ &transient_memory_resource };
    build_hashes_and_dependencies.reserve(m_builder_dependencies.size());
    for (const DependencyDescriptor& dependency_descriptor : m_builder_dependencies)
    {
        build_hashes_and_dependencies.push_back(
            std::pair{ dependency_descriptor.hash(),
                       dependency_descriptor.resolved_dependencies() }
        );
    }


    for (const auto [hash, resolved_dependencies] : build_hashes_and_dependencies)
    {
        fix_order_of_builders(hash, resolved_dependencies, transient_memory_resource);
    }
}

}   // namespace kiln::app
