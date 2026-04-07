module;

#include <cstdint>

export module kiln.res.ResourceID;

import kiln.util.Strong;

namespace kiln::res {

export using ResourceID = util::Strong<uint64_t, struct Tag>;

}   // namespace kiln::resource
