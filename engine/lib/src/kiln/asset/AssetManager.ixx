module;

#include <cstdint>
#include <memory_resource>
#include <type_traits>
#include <unordered_map>

export module kiln.asset.AssetManager;

import kiln.app.context.ContextBuilderInterface;
import kiln.app.memory.Arena;
import kiln.asset.asset_c;
import kiln.asset.AssetID;
import kiln.asset.AssetPool;
import kiln.asset.SharedAssetHandle;
import kiln.asset.WeakAssetHandle;
import kiln.util.containers.MoveOnlyAny;
import kiln.util.reflection;

namespace kiln::asset {

namespace internal {

export class AssetManagerBuilder;

}   // namespace internal

export class AssetManager {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;
    using Builder        = internal::AssetManagerBuilder;


    AssetManager() = default;
    explicit AssetManager(const allocator_type& allocator);
    AssetManager(const AssetManager&, const allocator_type& allocator) = delete;
    AssetManager(AssetManager&&, const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <asset_c Asset_T>
    [[nodiscard]]
    auto find(AssetID id) const -> WeakAssetHandle<Asset_T>;


    template <asset_c Asset_T, typename... Args_T>
        requires(std::is_constructible_v<Asset_T, Args_T && ...>)
    [[nodiscard]]
    auto try_emplace(AssetID id, Args_T&&... args) -> SharedAssetHandle<Asset_T>;

private:
    using ErasedAssetPool =
        util::BasicMoveOnlyAny<sizeof(AssetPool<int>), alignof(AssetPool<int>)>;

    std::pmr::unordered_map<uint64_t, ErasedAssetPool> m_asset_pools;
};

namespace internal {

export class AssetManagerBuilder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto build(app::Arena& arena) -> AssetManager
    {
        return AssetManager{ arena.pool_allocator() };
    }
};

}   // namespace internal

}   // namespace kiln::asset

namespace kiln::asset {

AssetManager::AssetManager(const allocator_type& allocator) : m_asset_pools{ allocator }
{
}

AssetManager::AssetManager(AssetManager&& other, const allocator_type& allocator)
    : m_asset_pools{ std::move(other.m_asset_pools), allocator }
{
}

auto AssetManager::get_allocator() const noexcept -> allocator_type
{
    return m_asset_pools.get_allocator();
}

template <asset_c Asset_T>
auto AssetManager::find(const AssetID id) const -> WeakAssetHandle<Asset_T>
{
    const auto iter{ m_asset_pools.find(util::hash_u64<Asset_T>()) };
    if (iter == m_asset_pools.cend())
    {
        return WeakAssetHandle<Asset_T>{};
    }

    return util::any_cast<AssetPool<Asset_T>>(iter->second).find(id);
}

template <asset_c Asset_T, typename... Args_T>
    requires(std::is_constructible_v<Asset_T, Args_T && ...>)
auto AssetManager::try_emplace(const AssetID id, Args_T&&... args)
    -> SharedAssetHandle<Asset_T>
{
    return m_asset_pools[util::hash_u64<Asset_T>()].try_emplace(
        id, std::forward<Args_T>(args)...
    );
}

}   // namespace kiln::asset
