module;

#include <memory>
#include <type_traits>
#include <utility>

export module kiln.asset.SharedAssetHandle;

import kiln.asset.asset_c;

namespace kiln::asset {

// TODO: use our own type instead of aliasing std::shared_ptr

export template <asset_c Asset_T>
using SharedAssetHandle = std::shared_ptr<Asset_T>;

export template <asset_c Asset_T, typename... Args_T>
    requires(std::is_constructible_v<Asset_T, Args_T && ...>)
auto make_shared_asset_handle(Args_T&&... args) -> SharedAssetHandle<Asset_T>
{
    return std::make_shared<Asset_T>(std::forward<Args_T>(args)...);
}

}   // namespace kiln::asset
