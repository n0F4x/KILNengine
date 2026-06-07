module;

#include <memory_resource>
#include <type_traits>
#include <unordered_map>

export module kiln.res.ResourcePool;

import kiln.res.resource_c;
import kiln.res.ResourceID;
import kiln.res.SharedResourceHandle;
import kiln.res.WeakResourceHandle;

namespace kiln::res {

export template <resource_c Resource_T>
class ResourcePool {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    ResourcePool() = default;
    explicit ResourcePool(const allocator_type& allocator);
    ResourcePool(const ResourcePool&, const allocator_type& allocator);
    ResourcePool(ResourcePool&&, const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto find(ResourceID id) const -> WeakResourceHandle<Resource_T>;


    template <typename... Args_T>
        requires(std::is_constructible_v<Resource_T, Args_T && ...>)
    auto try_emplace(ResourceID id, Args_T&&... args)
        -> std::pair<SharedResourceHandle<Resource_T>, bool>;

private:
    std::pmr::unordered_map<ResourceID, WeakResourceHandle<Resource_T>> m_resources;
};

}   // namespace kiln::res

namespace kiln::res {

template <resource_c Resource_T>
ResourcePool<Resource_T>::ResourcePool(const allocator_type& allocator)
    : m_resources{ allocator }
{
}

template <resource_c Resource_T>
ResourcePool<Resource_T>::ResourcePool(
    const ResourcePool&   other,
    const allocator_type& allocator
)
    : m_resources{ other.m_resources, allocator }
{
}

template <resource_c Resource_T>
ResourcePool<Resource_T>::ResourcePool(
    ResourcePool&&        other,
    const allocator_type& allocator
)
    : m_resources{ std::move(other.m_resources), allocator }
{
}

template <resource_c Resource_T>
auto ResourcePool<Resource_T>::get_allocator() const noexcept -> allocator_type
{
    return m_resources.get_allocator();
}

template <resource_c Resource_T>
auto ResourcePool<Resource_T>::find(const ResourceID id) const
    -> WeakResourceHandle<Resource_T>
{
    const auto iter{ m_resources.find(id) };
    if (iter == m_resources.cend())
    {
        return WeakResourceHandle<Resource_T>{};
    }

    return iter->second;
}

template <resource_c Resource_T>
template <typename... Args_T>
    requires(std::is_constructible_v<Resource_T, Args_T && ...>)
auto ResourcePool<Resource_T>::try_emplace(const ResourceID id, Args_T&&... args)
    -> std::pair<SharedResourceHandle<Resource_T>, bool>
{
    WeakResourceHandle<Resource_T>& contained_handle{ m_resources[id] };

    if (SharedResourceHandle<Resource_T> found_handle{ contained_handle.lock() };
        found_handle != nullptr)
    {
        return std::pair{ std::move(found_handle), false };
    }

    contained_handle
        = make_shared_resource_handle<Resource_T>(std::forward<Args_T>(args)...);

    return std::pair{ contained_handle.lock(), true };
}

}   // namespace kiln::res
