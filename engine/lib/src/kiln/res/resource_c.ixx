export module kiln.res.resource_c;

import kiln.util.concepts.naked;
import kiln.util.concepts.storable;

namespace kiln::res {

export template <typename T>
concept resource_c = util::naked_c<T> && util::storable_c<T>;

}   // namespace kiln::res
