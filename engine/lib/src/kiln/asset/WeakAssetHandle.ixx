module;

#include <memory>

export module kiln.asset.WeakAssetHandle;

import kiln.asset.asset_c;

namespace kiln::asset {

export template <asset_c Asset_T>
using WeakAssetHandle = std::weak_ptr<Asset_T>;

}   // namespace kiln::asset
