module;

#include <string_view>

export module kiln.asset.asset_id_from_string;

import kiln.asset.AssetID;
import kiln.util.hash;

namespace kiln::asset {

export [[nodiscard]]
constexpr auto asset_id_from_string(const std::string_view string) noexcept -> AssetID
{
    return AssetID{ util::hash_u64(string) };
}

}   // namespace kiln::asset
