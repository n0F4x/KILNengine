module;

#include <memory>

export module kiln.asset.SharedAssetHandle;

import kiln.asset.asset_c;

namespace kiln::asset {

export template <asset_c Asset_T>
using SharedAssetHandle = std::shared_ptr<Asset_T>;

}   // namespace kiln::asset
