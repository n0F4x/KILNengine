module;

#include <cstdint>

export module kiln.asset.AssetID;

import kiln.util.Strong;

namespace kiln::asset {

export using AssetID = util::Strong<uint64_t, struct Tag>;

}   // namespace kiln::asset
