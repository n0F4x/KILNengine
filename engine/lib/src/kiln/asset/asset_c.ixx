export module kiln.asset.asset_c;

import kiln.util.concepts.naked;
import kiln.util.concepts.storable;

namespace kiln::asset {

export template <typename T>
concept asset_c = util::naked_c<T> && util::storable_c<T>;

}   // namespace kiln::asset
