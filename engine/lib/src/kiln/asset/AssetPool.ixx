module;

#include <memory_resource>
#include <type_traits>
#include <unordered_map>

export module kiln.asset.AssetPool;

import kiln.asset.asset_c;
import kiln.asset.AssetID;
import kiln.asset.SharedAssetHandle;
import kiln.asset.WeakAssetHandle;

namespace kiln::asset {

export template <asset_c Asset_T>
class AssetPool {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    AssetPool() = default;
    explicit AssetPool(const allocator_type& allocator);
    AssetPool(const AssetPool&, const allocator_type& allocator);
    AssetPool(AssetPool&&, const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto find(AssetID id) const -> WeakAssetHandle<Asset_T>;


    template <typename... Args_T>
        requires(std::is_constructible_v<Asset_T, Args_T && ...>)
    auto try_emplace(AssetID id, Args_T&&... args)
        -> std::pair<SharedAssetHandle<Asset_T>, bool>;

private:
    std::pmr::unordered_map<AssetID, WeakAssetHandle<Asset_T>> m_assets;
};

}   // namespace kiln::asset

namespace kiln::asset {

template <asset_c Asset_T>
AssetPool<Asset_T>::AssetPool(const allocator_type& allocator) : m_assets{ allocator }
{
}

template <asset_c Asset_T>
AssetPool<Asset_T>::AssetPool(const AssetPool& other, const allocator_type& allocator)
    : m_assets{ other.m_assets, allocator }
{
}

template <asset_c Asset_T>
AssetPool<Asset_T>::AssetPool(AssetPool&& other, const allocator_type& allocator)
    : m_assets{ std::move(other.m_assets), allocator }
{
}

template <asset_c Asset_T>
auto AssetPool<Asset_T>::get_allocator() const noexcept -> allocator_type
{
    return m_assets.get_allocator();
}

template <asset_c Asset_T>
auto AssetPool<Asset_T>::find(const AssetID id) const -> WeakAssetHandle<Asset_T>
{
    const auto iter{ m_assets.find(id) };
    if (iter == m_assets.cend())
    {
        return WeakAssetHandle<Asset_T>{};
    }

    return iter->second;
}

template <asset_c Asset_T>
template <typename... Args_T>
    requires(std::is_constructible_v<Asset_T, Args_T && ...>)
auto AssetPool<Asset_T>::try_emplace(const AssetID id, Args_T&&... args)
    -> std::pair<SharedAssetHandle<Asset_T>, bool>
{
    WeakAssetHandle<Asset_T>& contained_handle{ m_assets[id] };

    if (SharedAssetHandle<Asset_T> found_handle{ contained_handle.lock() };
        found_handle != nullptr)
    {
        return std::pair{ std::move(found_handle), false };
    }

    contained_handle = make_shared_asset_handle<Asset_T>(std::forward<Args_T>(args)...);

    return std::pair{ contained_handle.lock(), true };
}

}   // namespace kiln::asset
