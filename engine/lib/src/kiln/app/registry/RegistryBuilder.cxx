module;

#include <memory_resource>
#include <utility>

module kiln.app.registry.RegistryBuilder;

import kiln.app.registry.EntryBuilderContainer;
import kiln.app.registry.EntryInjectionContainer;

kiln::app::RegistryBuilder::RegistryBuilder(
    RegistryBuilder&&     other,
    const allocator_type& allocator
)
    : m_injections{ std::move(other.m_injections), allocator },
      m_builders{ std::move(other.m_builders), allocator },
      m_registry{ std::move(other.m_registry), allocator }
{
}

kiln::app::RegistryBuilder::RegistryBuilder(const allocator_type& allocator)
    : m_injections{ allocator },
      m_builders{ allocator },
      m_registry{ allocator }
{
}

auto kiln::app::RegistryBuilder::get_allocator() const noexcept -> allocator_type
{
    return m_injections.get_allocator();
}

auto kiln::app::RegistryBuilder::build(
    std::pmr::memory_resource& transient_memory_resource
) && -> Registry
{
    std::move(m_injections).build(m_builders, m_registry, transient_memory_resource);
    std::move(m_builders).build(m_registry, transient_memory_resource);

    return std::move(m_registry);
}
