module;

#include <cstdint>
#include <memory_resource>
#include <type_traits>
#include <unordered_map>

export module kiln.res.ResourceManager;

import kiln.app.registry.EntryBuilderInterface;
import kiln.app.memory.MemoryArena;
import kiln.res.resource_c;
import kiln.res.ResourceID;
import kiln.res.ResourcePool;
import kiln.res.SharedResourceHandle;
import kiln.res.WeakResourceHandle;
import kiln.util.containers.MoveOnlyAny;
import kiln.util.reflection;

namespace kiln::res {

namespace internal {

export class ResourceManagerBuilder;

}   // namespace internal

export class ResourceManager {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;
    using Builder        = internal::ResourceManagerBuilder;


    ResourceManager() = default;
    explicit ResourceManager(const allocator_type& allocator);
    ResourceManager(const ResourceManager&, const allocator_type& allocator) = delete;
    ResourceManager(ResourceManager&&, const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <resource_c Resource_T>
    [[nodiscard]]
    auto find(ResourceID id) const -> WeakResourceHandle<Resource_T>;


    template <resource_c Resource_T, typename... Args_T>
        requires(std::is_constructible_v<Resource_T, Args_T && ...>)
    [[nodiscard]]
    auto try_emplace(ResourceID id, Args_T&&... args) -> SharedResourceHandle<Resource_T>;

private:
    using ErasedResourcePool
        = util::BasicMoveOnlyAny<sizeof(ResourcePool<int>), alignof(ResourcePool<int>)>;

    std::pmr::unordered_map<uint64_t, ErasedResourcePool> m_resource_pools;
};

namespace internal {

export class ResourceManagerBuilder : public app::EntryBuilderInterface {
public:
    [[nodiscard]]
    static auto build(app::MemoryArena& memory_arena) -> ResourceManager
    {
        return ResourceManager{ memory_arena.pool_allocator() };
    }
};

}   // namespace internal

}   // namespace kiln::res

namespace kiln::res {

ResourceManager::ResourceManager(const allocator_type& allocator)
    : m_resource_pools{ allocator }
{
}

ResourceManager::ResourceManager(ResourceManager&& other, const allocator_type& allocator)
    : m_resource_pools{ std::move(other.m_resource_pools), allocator }
{
}

auto ResourceManager::get_allocator() const noexcept -> allocator_type
{
    return m_resource_pools.get_allocator();
}

template <resource_c Resource_T>
auto ResourceManager::find(const ResourceID id) const -> WeakResourceHandle<Resource_T>
{
    const auto iter{ m_resource_pools.find(util::hash_u64<Resource_T>()) };
    if (iter == m_resource_pools.cend())
    {
        return WeakResourceHandle<Resource_T>{};
    }

    return util::any_cast<ResourcePool<Resource_T>>(iter->second).find(id);
}

template <resource_c Resource_T, typename... Args_T>
    requires(std::is_constructible_v<Resource_T, Args_T && ...>)
auto ResourceManager::try_emplace(const ResourceID id, Args_T&&... args)
    -> SharedResourceHandle<Resource_T>
{
    return m_resource_pools[util::hash_u64<Resource_T>()]
        .try_emplace(id, std::forward<Args_T>(args)...);
}

}   // namespace kiln::res
